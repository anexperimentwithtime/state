// Copyright (C) 2025 Ian Torres <iantorres@outlook.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.


#include <aewt/session.hpp>
#include <aewt/state.hpp>
#include <aewt/logger.hpp>
#include <aewt/subscription.hpp>
#include <aewt/request.hpp>

#include <boost/uuid/random_generator.hpp>
#include <boost/json/serialize.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <ranges>
#include <unordered_set>

#include <aewt/utils.hpp>

namespace aewt {
    state::state()
        : id_(boost::uuids::random_generator()()), created_at_(std::chrono::system_clock::now()) {
        LOG_INFO("state {} allocated", to_string(id_));
    }

    state::~state() {
        LOG_INFO("state {} released", to_string(id_));
    }

    boost::uuids::uuid state::get_id() const { return id_; }

    std::chrono::system_clock::time_point state::get_created_at() const {
        return created_at_;
    }

    std::vector<std::shared_ptr<session> > state::get_sessions() const {
        std::shared_lock _lock(sessions_mutex_);
        std::vector<std::shared_ptr<session> > _result;
        _result.reserve(sessions_.size());

        for (auto &_session: sessions_ | std::views::values)
            _result.push_back(_session);

        return _result;
    }

    std::vector<boost::uuids::uuid> state::get_clients_by_session(const boost::uuids::uuid session_id) const {
        std::shared_lock _lock(clients_mutex_);

        const auto &_index = clients_.get<clients_by_session>();
        auto [_begin, _end] = _index.equal_range(session_id);

        std::vector<boost::uuids::uuid> _result;
        _result.reserve(std::distance(_begin, _end));

        for (auto it = _begin; it != _end; ++it)
            _result.push_back((*it)->get_id());

        return _result;
    }

    std::vector<std::shared_ptr<client> > state::get_clients() const {
        std::shared_lock _lock(clients_mutex_);

        const auto &_index = clients_.get<clients_by_client>();

        std::vector<std::shared_ptr<client> > _result;
        _result.reserve(clients_.size());

        for (const auto &_client: _index)
            _result.push_back(_client);

        return _result;
    }

    bool state::get_client_exists_on_session(const boost::uuids::uuid client_id,
                                             const boost::uuids::uuid session_id) const {
        std::shared_lock _lock(clients_mutex_);

        const auto &_index = clients_.get<clients_by_client_session>();
        return _index.find(boost::make_tuple(client_id, session_id)) != _index.end();
    }

    bool state::get_client_exists(const boost::uuids::uuid client_id) const {
        std::shared_lock _lock(clients_mutex_);

        const auto &_index = clients_.get<clients_by_client>();
        return _index.find(client_id) != _index.end();
    }

    std::optional<std::shared_ptr<session> > state::get_session(
        const boost::uuids::uuid id) const {
        std::shared_lock _lock(sessions_mutex_);

        const auto _iterator = sessions_.find(id);
        if (_iterator == sessions_.end()) {
            return std::nullopt;
        }
        return _iterator->second;
    }

    std::optional<std::shared_ptr<client> > state::get_client(
        const boost::uuids::uuid id) const {
        std::shared_lock _lock(clients_mutex_);

        auto &_index = clients_.get<clients_by_client>();

        const auto _iterator = _index.find(id);

        if (_iterator == _index.end()) {
            return std::nullopt;
        }
        return *_iterator;
    }

    bool state::add_session(std::shared_ptr<session> session) {
        std::unique_lock _lock(sessions_mutex_);
        auto [_, _inserted] = sessions_.emplace(session->get_id(), session);
        return _inserted;
    }

    bool state::remove_session(const boost::uuids::uuid id) {
        std::unique_lock _lock(sessions_mutex_);
        return sessions_.erase(id) > 0;
    }

    bool state::add_client(const std::shared_ptr<client> &client) {
        std::unique_lock _lock(clients_mutex_);

        auto &_index = clients_.get<clients_by_client_session>();
        auto [_it, _inserted] =
                _index.insert(client);

        return _inserted;
    }

    bool state::remove_client(const boost::uuids::uuid client_id) {
        std::unique_lock _lock(clients_mutex_);

        auto &_index = clients_.get<clients_by_client>();
        auto [_begin, _end] = _index.equal_range(client_id);

        const std::size_t _count = std::distance(_begin, _end);
        _index.erase(_begin, _end);
        return _count;
    }

    bool state::subscribe(const boost::uuids::uuid &session_id, const boost::uuids::uuid &client_id,
                          const std::string &channel) {
        std::unique_lock _lock(subscriptions_mutex_);

        auto &_index =
                subscriptions_.get<subscriptions_by_session_client_channel>();

        auto [_it, _inserted] =
                _index.insert(subscription{session_id, client_id, channel});

        return _inserted;
    }

    bool state::unsubscribe(const boost::uuids::uuid &session_id, const boost::uuids::uuid &client_id,
                            const std::string &channel) {
        std::unique_lock _lock(subscriptions_mutex_);

        auto &_index =
                subscriptions_.get<subscriptions_by_session_client_channel>();

        const auto _iterator = _index.find(
            boost::make_tuple(session_id, client_id, channel)
        );

        if (_iterator == _index.end())
            return false;

        _index.erase(_iterator);
        return true;
    }

    bool state::is_subscribed(const boost::uuids::uuid &session_id, const boost::uuids::uuid &client_id,
                              const std::string &channel) {
        std::shared_lock _lock(subscriptions_mutex_);

        const auto &_index =
                subscriptions_.get<subscriptions_by_session_client_channel>();

        return _index.find(
                   boost::make_tuple(session_id, client_id, channel)
               ) != _index.end();
    }

    std::size_t state::unsubscribe_all_client(const boost::uuids::uuid &client_id) {
        std::unique_lock _lock(subscriptions_mutex_);

        auto &_index = subscriptions_.get<subscriptions_by_client>();

        const auto [_begin, _end] = _index.equal_range(client_id);

        const std::size_t _count = std::distance(_begin, _end);
        _index.erase(_begin, _end);
        return _count;
    }

    std::size_t state::unsubscribe_all_session(const boost::uuids::uuid &session_id) {
        std::unique_lock _lock(subscriptions_mutex_);

        auto &_index = subscriptions_.get<subscriptions_by_session>();

        const auto [_begin, _end] = _index.equal_range(session_id);

        const std::size_t _count = std::distance(_begin, _end);
        _index.erase(_begin, _end);
        return _count;
    }

    std::size_t state::broadcast_to_sessions(const request &request, const boost::uuids::uuid session_id,
                                             const boost::uuids::uuid client_id,
                                             const boost::json::object &data) const {
        const auto _data = make_broadcast_request_object(request, session_id, client_id, data);

        return send_to_others_sessions(_data, session_id);
    }

    std::size_t state::broadcast_to_clients(const request &request, const boost::uuids::uuid session_id,
                                            const boost::uuids::uuid client_id, const boost::json::object &data) const {
        const auto _data = std::make_shared<boost::json::object>(
            make_broadcast_request_object(request, session_id, client_id, data)
        );

        return send_to_others_clients(_data, client_id);
    }

    std::size_t state::publish(const boost::uuids::uuid transaction_id, const boost::uuids::uuid session_id,
                               const boost::uuids::uuid client_id, const std::string &channel,
                               boost::json::object data) {
        std::shared_lock _lock(subscriptions_mutex_);

        const auto &_by_channel = subscriptions_.get<subscriptions_by_channel>();
        auto [_begin, _end] = _by_channel.equal_range(channel);

        std::unordered_set<boost::uuids::uuid> _receivers;
        auto _count = 0;
        _receivers.reserve(std::distance(_begin, _end));
        for (auto _iterator = _begin; _iterator != _end; ++_iterator) {
            _receivers.insert(_iterator->session_id_);
            _count++;
        }

        const auto _data = boost::json::object({
            {"transaction_id", to_string(transaction_id)},
            {"action", "publish"},
            {
                "params", {
                    {"session_id", to_string(session_id)},
                    {"client_id", to_string(client_id)},
                    {"channel", channel},
                    {"payload", data},
                    {"count", _count}
                }
            }
        });

        auto const _message = std::make_shared<std::string const>(serialize(_data));

        for (const auto &_session_id: _receivers) {
            if (auto _session = get_session(_session_id); _session.has_value()) {
                const auto &_value = _session.value();
                _value->send(_message);
            }
        }

        return _count;
    }

    std::vector<subscription> state::get_client_subscriptions(const boost::uuids::uuid client_id) {
        std::shared_lock _lock(subscriptions_mutex_);

        const auto &_by_client = subscriptions_.get<subscriptions_by_client>();
        auto [_begin, _end] = _by_client.equal_range(client_id);

        std::vector<subscription> _subscriptions;
        _subscriptions.reserve(std::distance(_begin, _end));
        for (auto _iterator = _begin; _iterator != _end; ++_iterator) {
            _subscriptions.push_back(*_iterator);
        }
        return _subscriptions;
    }

    bool state::send(const boost::uuids::uuid transaction_id, const boost::uuids::uuid session_id,
                     const boost::uuids::uuid sender_id, const boost::uuids::uuid receiver_id,
                     const boost::json::object &data) const {
        if (const auto _client = get_client(receiver_id); _client.has_value()) {
            const auto _client_value = _client.value();
            if (const auto _receiver = get_session(_client_value->get_session_id()); _receiver.has_value()) {
                const auto _data = boost::json::object({
                    {"transaction_id", to_string(transaction_id)},
                    {"action", "send"},
                    {"session_id", to_string(session_id)},
                    {"sender_id", to_string(sender_id)},
                    {"receiver_id", to_string(receiver_id)},
                    {"payload", data},
                });

                auto const _message = std::make_shared<std::string const>(serialize(_data));
                const auto &_session = _receiver.value();
                _session->send(_message);
                return true;
            }
        }

        return false;
    }

    bool state::push_client(const std::shared_ptr<client> &client) {
        std::unique_lock _lock(clients_mutex_);

        auto &_index = clients_.get<clients_by_client_session>();
        auto [_it, _inserted] =
                _index.insert(client);

        return _inserted;
    }

    std::size_t state::send_to_others_sessions(const boost::json::object &data,
                                               const boost::uuids::uuid except) const {
        auto _sessions = get_sessions();

        std::string _output = serialize(data);
        auto const _message = std::make_shared<std::string const>(_output);

        std::size_t _count = 0;
        for (const auto &_session: _sessions) {
            if (_session->get_id() == except)
                continue;

            _session->send(_message);
            _count++;
        }

        return _count;
    }

    std::size_t state::send_to_others_clients(const std::shared_ptr<boost::json::object> &data,
                                              const boost::uuids::uuid except) const {
        auto _clients = get_clients();

        std::size_t _count = 0;
        for (const auto &_client: _clients) {
            if (_client->get_id() == except)
                continue;

            // _client->send(data);
            _count++;
        }

        return _count;
    }
} // namespace aewt
