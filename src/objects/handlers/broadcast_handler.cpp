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

#include <aewt/state.hpp>
#include <aewt/session.hpp>
#include <aewt/request.hpp>

#include <aewt/validators/broadcast_validator.hpp>

#include <aewt/utils.hpp>

namespace aewt::handlers {
    std::size_t broadcast_to_sessions(const bool is_local, const std::shared_ptr<state> &state, const request &request,
                                      const boost::uuids::uuid session_id, const boost::uuids::uuid client_id,
                                      const boost::json::object &payload) {
        return is_local
                   ? state->broadcast_to_sessions(request, session_id, client_id, payload)
                   : std::size_t{0};
    }

    void broadcast_handler(const request &request) {
        auto &_state = request.state_;

        if (validators::broadcast_validator(request)) {
            auto &_params = get_params(request);
            const auto &_payload = get_param_as_object(_params, "payload");

            const auto _clients_count = _state->broadcast_to_clients(
                request,
                request.session_id_,
                request.client_id_,
                _payload
            );

            const auto _sessions_count = broadcast_to_sessions(
                request.is_local_,
                _state,
                request,
                request.session_id_,
                request.client_id_,
                _payload
            );

            const auto _count = _clients_count + _sessions_count;

            const auto _status = get_status(_count > 0);

            next(request, _status, {
                     {"clients_count", _clients_count},
                     {"sessions_count", _sessions_count},
                     {"count", _count}
                 });
        }
    }
}
