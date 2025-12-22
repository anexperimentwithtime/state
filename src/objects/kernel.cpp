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
#include <aewt/response.hpp>
#include <aewt/session.hpp>
#include <aewt/state.hpp>
#include <aewt/validator.hpp>

#include <aewt/handlers/ping_handler.hpp>
#include <aewt/handlers/whoami_handler.hpp>

#include <aewt/handlers/client_handler.hpp>
#include <aewt/handlers/clients_handler.hpp>
#include <aewt/handlers/client_join_handler.hpp>
#include <aewt/handlers/client_leave_handler.hpp>
#include <aewt/handlers/client_exists_handler.hpp>

#include <aewt/handlers/session_handler.hpp>
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

#include <boost/uuid/uuid_io.hpp>
#include <boost/json/serialize.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/lexical_cast.hpp>

namespace aewt {
    std::shared_ptr<response> kernel(const std::shared_ptr<state> &state,
                                     const std::shared_ptr<session> &session,
                                     boost::json::object data) {
        boost::ignore_unused(state);

        auto _response = std::make_shared<response>();
        if (const validator _validator(data); _validator.get_passed()) {
            const auto transaction_id = boost::lexical_cast<boost::uuids::uuid>(std::string{
                data.at("transaction_id").as_string()
            });
            if (const std::string _action{data.at("action").as_string()}; _action == "ping") {
                handlers::ping_handler(transaction_id, _response);
            } else if (_action == "subscribe") {
                handlers::subscribe_handler(transaction_id, _response, state, session, data);
            } else if (_action == "is_subscribed") {
                handlers::is_subscribed_handler(transaction_id, _response, state, session, data);
            } else if (_action == "unsubscribe") {
                handlers::unsubscribe_handler(transaction_id, _response, state, session, data);
            } else if (_action == "unsubscribe_all_client") {
                handlers::unsubscribe_all_client_handler(transaction_id, _response, state, session, data);
            } else if (_action == "unsubscribe_all_session") {
                handlers::unsubscribe_all_session_handler(transaction_id, _response, state, session, data);
            } else if (_action == "broadcast") {
                handlers::broadcast_handler(transaction_id, _response, state, session, data);
            } else if (_action == "publish") {
                handlers::publish_handler(transaction_id, _response, state, session, data);
            } else if (_action == "send") {
                handlers::send_handler(transaction_id, _response, state, session, data);
            } else if (_action == "client_join") {
                handlers::client_join_handler(transaction_id, _response, state, session, data);
            } else if (_action == "client") {
                handlers::client_handler(transaction_id, _response, state, session, data);
            } else if (_action == "client_leave") {
                handlers::client_leave_handler(transaction_id, _response, state, session, data);
            } else if (_action == "session_clients") {
                handlers::session_clients_handler(transaction_id, _response, state, session, data);
            } else if (_action == "clients") {
                handlers::clients_handler(transaction_id, _response, state, session);
            } else if (_action == "client_exists") {
                handlers::client_exists_handler(transaction_id, _response, state, session, data);
            } else if (_action == "session") {
                handlers::session_handler(transaction_id, _response, state, session, data);
            } else if (_action == "session_client_exists") {
                handlers::session_client_exists_handler(transaction_id, _response, state, session, data);
            } else if (_action == "whoami") {
                handlers::whoami_handler(transaction_id, _response, session);
            } else {
                handlers::unimplemented_handler(transaction_id, _response);
            }
        } else {
            if (data.contains("transaction_id") && data.at("transaction_id").is_string() && validator::is_uuid(
                    data.at("transaction_id").as_string().c_str())) {
                _response->mark_as_failed(
                    boost::lexical_cast<boost::uuids::uuid>(std::string{data.at("transaction_id").as_string()}),
                    "unprocessable entity", _validator.get_bag());
            } else {
                _response->mark_as_failed(boost::uuids::uuid {}, "unprocessable entity", _validator.get_bag());
            }
        }
        _response->mark_as_processed();
        return _response;
    }
} // namespace aewt
