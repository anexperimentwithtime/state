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

#include <aewt/utils.hpp>

#include <aewt/request.hpp>
#include <aewt/response.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>

namespace aewt {
    void next(const request &request, const char *status, const boost::json::object &data) {
        request.response_->set_data(
            request.transaction_id_,
            status,
            request.timestamp_,
            data);
    }

    void mark_as_invalid(const request &request, const char *field, const char *argument) {
        request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                          {{field, argument}});
    }

    std::string get_param_as_string(const boost::json::object &params, const char *field) {
        return std::string{params.at(field).as_string()};
    }

    const boost::json::object &get_params(const request &request) {
        return request.data_.at("params").as_object();
    }

    const boost::json::object &get_param_as_object(const boost::json::object &params, const char *field) {
        return params.at(field).as_object();
    }

    const boost::json::value &get_params_as_value(const request &request) {
        return request.data_.at("params");
    }

    boost::uuids::uuid get_param_as_id(const boost::json::object &params, const char *field) {
        return boost::lexical_cast<boost::uuids::uuid>(std::string{params.at(field).as_string()});
    }

    boost::json::array make_array_of_ids(const std::vector<boost::uuids::uuid> &vector) {
        boost::json::array _array;
        for (const auto &_item: vector) {
            _array.push_back(to_string(_item).data());
        }
        return _array;
    }

    boost::json::array make_channels_array_of_subscriptions(const std::vector<subscription> &subscriptions) {
        boost::json::array _array;
        _array.reserve(subscriptions.size());
        for (const auto &_subscription: subscriptions) {
            _array.emplace_back(_subscription.channel_);
        }
        return _array;
    }

    boost::json::array make_array_of_clients_ids(const std::vector<boost::uuids::uuid> &clients) {
        boost::json::array _array;
        for (const auto &_client: clients) {
            _array.push_back(to_string(_client).data());
        }
        return _array;
    }

    boost::json::object make_client_object(const std::shared_ptr<client> &client,
                                           const boost::json::array &subscriptions) {
        boost::json::object _data = {
            {"id", to_string(client->get_id())},
            {"session_id", to_string(client->get_session_id())},
            {"subscriptions", subscriptions},
            {"is_local", client->get_is_local()},
        };

        if (const auto &_socket_optional = client->get_socket(); _socket_optional.has_value()) {
            if (auto &_socket = _socket_optional.value(); _socket.is_open()) {
                _data["ip"] = _socket.remote_endpoint().address().to_string();
                _data["port"] = _socket.remote_endpoint().port();
            } else {
                _data["ip"] = nullptr;
                _data["port"] = nullptr;
            }
        } else {
            _data["ip"] = nullptr;
            _data["port"] = nullptr;
        }

        return _data;
    }

    boost::json::object make_session_object(const std::shared_ptr<session> &session) {
        const auto &_socket = session->get_socket();

        boost::json::object _data = {
            {"id", to_string(session->get_id())},
            {"is_open", _socket.is_open()},
        };

        if (_socket.is_open()) {
            const auto _remote_endpoint = _socket.remote_endpoint();
            _data["ip"] = _remote_endpoint.address().to_string();
            _data["port"] = _remote_endpoint.port();
        } else {
            _data["ip"] = nullptr;
            _data["port"] = nullptr;
        }

        return _data;
    }

    const char *get_status(const bool gate, const char *on_true, const char *on_false) {
        return gate
                   ? on_true
                   : on_false;
    }
} // namespace aewt
