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

#include <aewt/handlers/is_subscribed_handler.hpp>

#include <aewt/response.hpp>
#include <aewt/session.hpp>
#include <aewt/state.hpp>

#include <aewt/validators/is_subscribed_validator.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace aewt::handlers {
    void is_subscribed_handler(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response,
                                 const std::shared_ptr<state> &state,
                                 const std::shared_ptr<session> &session, const boost::json::object &data) {
        boost::ignore_unused(session);

        if (validators::is_subscribed_validator(transaction_id, response, data)) {
            const auto _client_id = boost::lexical_cast<boost::uuids::uuid>(
                std::string{data.at("params").as_object().at("client_id").as_string()});
            const auto _session_id = boost::lexical_cast<boost::uuids::uuid>(
                std::string{data.at("params").as_object().at("session_id").as_string()});
            const std::string _channel{data.at("params").as_object().at("channel").as_string()};
            const auto _timestamp = std::chrono::system_clock::now();
            const bool _success = state->is_subscribed(_session_id, _client_id, _channel);
            response->set_data(transaction_id, _success ? "yes" : "no", {
                                   {"timestamp", _timestamp.time_since_epoch().count()}
                               });
        }
    }
}
