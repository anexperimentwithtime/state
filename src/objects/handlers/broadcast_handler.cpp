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
    void broadcast_handler(const request &request) {
        if (validators::broadcast_validator(request)) {
            auto &_params = get_params(request);
            const auto &_client_id = get_param_as_id(_params, "client_id");
            const auto &_session_id = get_param_as_id(_params, "session_id");
            const auto &_payload = get_param_as_object(_params, "payload");

            const bool _is_local = request.session_->get_id() == _session_id;

            const std::size_t _clients_count = request.state_->broadcast_to_clients(
                request,
                _session_id,
                _client_id,
                _payload
            );

            const std::size_t _sessions_count = _is_local
                                                    ? request.state_->broadcast_to_sessions(
                                                        request,
                                                        _session_id,
                                                        _client_id,
                                                        _payload
                                                    )
                                                    : 0;

            const std::size_t _count = _clients_count + _sessions_count;

            const auto _status = get_status(_count > 0);

            next(request, _status, {
                     {"clients_count", _clients_count},
                     {"sessions_count", _sessions_count},
                     {"count", _count}
                 });
        }
    }
}
