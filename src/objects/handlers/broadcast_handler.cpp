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

#include <aewt/handlers/broadcast_handler.hpp>

#include <aewt/response.hpp>
#include <aewt/state.hpp>
#include <aewt/session.hpp>

#include <aewt/validators/broadcast_validator.hpp>

#include <aewt/utils.hpp>

namespace aewt::handlers {
    void broadcast_handler(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response,
                           const std::shared_ptr<state> &state, const std::shared_ptr<session> &session,
                           const boost::json::object &data, const long timestamp) {
        if (validators::broadcast_validator(transaction_id, response, data, timestamp)) {
            auto _params = data.at("params").as_object();
            const auto _client_id = GET_PARAM_AS_ID(_params, "client_id");
            const auto _payload = _params.at("payload").as_object();

            const std::size_t _count = state->broadcast(
                transaction_id,
                session->get_id(),
                _client_id,
                _payload
            );

            const auto _status = _count > 0 ? "ok" : "no effect";

            response->set_data(
                transaction_id,
                _status,
                timestamp,
                {
                    {"count", _count}
                });
        }
    }
}
