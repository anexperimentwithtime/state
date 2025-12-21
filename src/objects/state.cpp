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

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <ranges>

namespace aewt {
    state::state()
        : id_(generator_()), created_at_(std::chrono::system_clock::now()) {
        LOG_INFO("state {} allocated", to_string(id_));
    }

    state::~state() { LOG_INFO("state {} released", to_string(id_)); }

    boost::uuids::random_generator state::get_generator() const {
        return generator_;
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

    std::optional<std::shared_ptr<session> > state::get_session(
        const boost::uuids::uuid id) const {
        std::shared_lock _lock(sessions_mutex_);

        const auto _iterator = sessions_.find(id);
        if (_iterator == sessions_.end()) {
            return std::nullopt;
        }
        return _iterator->second;
    }

    void state::add_session(std::shared_ptr<session> session) {
        std::unique_lock _lock(sessions_mutex_);
        sessions_.emplace(session->get_id(), std::move(session));
    }

    void state::remove_session(const boost::uuids::uuid id) {
        std::unique_lock _lock(sessions_mutex_);
        sessions_.erase(id);
    }

    bool state::subscribe(const boost::uuids::uuid &session_id, const boost::uuids::uuid &client_id,
        const std::string &channel) {
        std::unique_lock _lock(subscriptions_mutex_);

        auto& _index =
            subscriptions_.get<subscriptions_by_session_client_channel>();

        auto [_it, _inserted] =
            _index.insert(subscription{session_id, client_id, channel});

        return _inserted;
    }

    bool state::unsubscribe(const boost::uuids::uuid &session_id, const boost::uuids::uuid &client_id,
        const std::string &channel) {

        std::unique_lock _lock(subscriptions_mutex_);

        auto& _index =
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

        const auto& _index =
            subscriptions_.get<subscriptions_by_session_client_channel>();

        return _index.find(
            boost::make_tuple(session_id, client_id, channel)
        ) != _index.end();
    }

    std::size_t state::unsubscribe_all_client(const boost::uuids::uuid &client_id) {
        std::unique_lock _lock(subscriptions_mutex_);

        auto& _index = subscriptions_.get<subscriptions_by_client>();

        const auto _range = _index.equal_range(client_id);

        const std::size_t _count = std::distance(_range.first, _range.second);
        _index.erase(_range.first, _range.second);
        return _count;
    }

    std::size_t state::unsubscribe_all_session(const boost::uuids::uuid &session_id) {
        std::unique_lock _lock(subscriptions_mutex_);

        auto& _index = subscriptions_.get<subscriptions_by_session>();

        const auto _range = _index.equal_range(session_id);

        const std::size_t _count = std::distance(_range.first, _range.second);
        _index.erase(_range.first, _range.second);
        return _count;
    }

    std::size_t state::publish(boost::uuids::uuid transaction_id, boost::uuids::uuid session_id,
        boost::uuids::uuid client_id, const std::string &channel, boost::json::object data) {
        std::shared_lock _lock(subscriptions_mutex_);

        const auto& _by_channel = subscriptions_.get<subscriptions_by_channel>();
        auto _range = _by_channel.equal_range(channel);

        std::vector<boost::uuids::uuid> _sessions;
        for (auto _it = _range.first; _it != _range.second; ++_it) {
            LOG_INFO("WTF: {}", to_string(_it->session_id_));
            _sessions.push_back(_it->session_id_);
        }

        std::size_t _count = 0;
        std::vector<std::shared_ptr<session>> _receivers;
        for (const auto& _session_id : _sessions) {

            LOG_INFO("SESSION: {}", to_string(_session_id));
            if (auto _it = sessions_.find(_session_id); _it != sessions_.end()) {
                auto _session = _it->second;
                _receivers.push_back(_session);
                _count++;
            }
        }

        const auto _data = std::make_shared<boost::json::object>(boost::json::object({
            {"transaction_id", to_string(transaction_id)},
            {"action", "broadcast"},
            {"session_id", to_string(session_id)},
            {"client_id", to_string(client_id)},
            {"payload", data},
            {"count", _count}
        }));

        for (const auto& _receiver : _receivers) {
            _receiver->send(_data);
        }

        return _count;
    }
} // namespace aewt
