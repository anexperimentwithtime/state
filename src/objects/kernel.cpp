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

#include <fmt/format.h>

#include <aewt/kernel.hpp>
#include <aewt/request.hpp>
#include <aewt/response.hpp>
#include <aewt/session.hpp>
#include <aewt/state.hpp>
#include <aewt/logger.hpp>
#include <aewt/validator.hpp>

#include <aewt/handlers/ping_handler.hpp>

#include <aewt/handlers/client_handler.hpp>
#include <aewt/handlers/clients_handler.hpp>
#include <aewt/handlers/client_join_handler.hpp>
#include <aewt/handlers/client_leave_handler.hpp>
#include <aewt/handlers/client_exists_handler.hpp>

#include <aewt/handlers/session_handler.hpp>
#include <aewt/handlers/session_join_handler.hpp>
#include <aewt/handlers/session_leave_handler.hpp>
#include <aewt/handlers/session_clients_handler.hpp>
#include <aewt/handlers/session_client_exists_handler.hpp>

#include <aewt/handlers/subscribe_handler.hpp>

#include <aewt/handlers/unsubscribe_handler.hpp>
#include <aewt/handlers/unsubscribe_all_client_handler.hpp>
#include <aewt/handlers/unsubscribe_all_session_handler.hpp>

#include <aewt/handlers/is_subscribed_handler.hpp>

#include <aewt/handlers/broadcast_handler.hpp>
#include <aewt/handlers/publish_handler.hpp>
#include <aewt/handlers/send_handler.hpp>

#include <aewt/handlers/unimplemented_handler.hpp>

#include <aewt/utils.hpp>

#include <boost/json/serialize.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace aewt {
    std::shared_ptr<response> kernel(const std::shared_ptr<state> &state,
                                     const boost::json::object &data) {
        boost::ignore_unused(state);

        const auto _timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        const auto _kernel_id = boost::uuids::random_generator()();

        LOG_INFO("kernel {} started", to_string(_kernel_id));

        auto _response = std::make_shared<response>();
        if (const validator _validator(data); _validator.get_passed()) {
            const auto &_params = data.at("params").as_object();
            const auto &_session_id = get_param_as_id(_params, "session_id");
            const auto &_client_id = get_param_as_id(_params, "client_id");
            const auto _request = request{
                .transaction_id_ = get_param_as_id(data, "transaction_id"),
                .response_ = _response,
                .state_ = state,
                .session_id_ = _session_id,
                .client_id_ = _client_id,
                .data_ = data,
                .timestamp_ = _timestamp,
                .is_local_ = _session_id == state->get_id()
            };

            const std::string _action{data.at("action").as_string()};

            LOG_INFO("kernel {} action [{}] invoked session_id={} client_id={} data={}", to_string(_kernel_id), _action,
                     to_string(_session_id), to_string(_client_id), serialize(_request.data_));

            if (_action == "ping") {
                handlers::ping_handler(_request);
            } else if (_action == "subscribe") {
                handlers::subscribe_handler(_request);
            } else if (_action == "is_subscribed") {
                handlers::is_subscribed_handler(_request);
            } else if (_action == "unsubscribe") {
                handlers::unsubscribe_handler(_request);
            } else if (_action == "unsubscribe_all_client") {
                handlers::unsubscribe_all_client_handler(_request);
            } else if (_action == "unsubscribe_all_session") {
                handlers::unsubscribe_all_session_handler(_request);
            } else if (_action == "broadcast") {
                handlers::broadcast_handler(_request);
            } else if (_action == "publish") {
                handlers::publish_handler(_request);
            } else if (_action == "send") {
                handlers::send_handler(_request);
            } else if (_action == "client") {
                handlers::client_handler(_request);
            } else if (_action == "client_join") {
                handlers::client_join_handler(_request);
            } else if (_action == "client_leave") {
                handlers::client_leave_handler(_request);
            } else if (_action == "clients") {
                handlers::clients_handler(_request);
            } else if (_action == "client_exists") {
                handlers::client_exists_handler(_request);
            } else if (_action == "session") {
                handlers::session_handler(_request);
            } else if (_action == "session_join") {
                handlers::session_join_handler(_request);
            } else if (_action == "session_leave") {
                handlers::session_leave_handler(_request);
            } else if (_action == "session_client_exists") {
                handlers::session_client_exists_handler(_request);
            } else if (_action == "session_clients") {
                handlers::session_clients_handler(_request);
            } else {
                handlers::unimplemented_handler(_request);
            }
        } else {
            if (data.contains("transaction_id") && data.at("transaction_id").is_string() && validator::is_uuid(
                    data.at("transaction_id").as_string().c_str())) {
                _response->mark_as_failed(
                    get_param_as_id(data, "transaction_id"),
                    "unprocessable entity", _timestamp, _validator.get_bag());
            } else {
                _response->mark_as_failed(boost::uuids::uuid{}, "unprocessable entity", _timestamp,
                                          _validator.get_bag());
            }
        }
        _response->mark_as_processed();

        LOG_INFO("kernel {} finished", to_string(_kernel_id));
        return _response;
    }
} // namespace aewt
