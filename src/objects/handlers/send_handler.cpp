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

#include <aewt/handlers/send_handler.hpp>

#include <aewt/response.hpp>
#include <aewt/state.hpp>
#include <aewt/session.hpp>

#include <aewt/validators/send_validator.hpp>

#include <aewt/utils.hpp>

namespace aewt::handlers {
    void send_handler(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response,
                      const std::shared_ptr<state> &state, const std::shared_ptr<session> &session,
                      const boost::json::object &data, const long timestamp) {
        if (validators::send_validator(transaction_id, response, data, timestamp)) {
            auto _params = data.at("params").as_object();
            const auto _sender_id = GET_PARAM_AS_ID(_params, "sender_id");
            const auto _receiver_id = GET_PARAM_AS_ID(_params, "receiver_id");
            const auto _payload = _params.at("payload").as_object();

            const bool _success = state->send(transaction_id, session->get_id(), _sender_id, _receiver_id, _payload);

            const auto _status = _success ? "ok" : "no effect";

            response->set_data(transaction_id, _status, timestamp);
        }
    }
}
