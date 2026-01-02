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
#include <aewt/logger.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace aewt::handlers {
    void broadcast_handler(const request &request) {
        auto &_state = request.state_;

        if (validators::broadcast_validator(request)) {
            auto &_params = get_params(request);
            const auto &_payload = get_param_as_object(_params, "payload");

            std::size_t _count = 0;

            switch (request.context_) {
                case on_client: {
                    _count = _state->broadcast_to_clients(
                        request,
                        _state->get_id(),
                        request.entity_id_,
                        _payload
                    );

                    const auto _ = request.state_->broadcast_to_sessions(
                        request,
                        request.entity_id_,
                        _payload
                    );
                    boost::ignore_unused(_);

                    LOG_INFO("state_id=[{}] action=[broadcast] context=[{}] client_id=[{}] count=[{}] size=[{}]",
                             to_string(_state->get_id()), kernel_context_to_string(request.context_),
                             to_string(request.entity_id_), _count, _payload.size());

                    break;
                }
                case on_session: {
                    const auto &_client_id = get_param_as_id(_params, "client_id");
                    _count = _state->broadcast_to_clients(
                        request,
                        _state->get_id(),
                        _client_id,
                        _payload
                    );

                    LOG_INFO(
                        "state_id=[{}] action=[broadcast] context=[{}] session_id=[{}] client_id=[{}] count=[{}] size=[{}]",
                        to_string(_state->get_id()), kernel_context_to_string(request.context_),
                        to_string(request.entity_id_), to_string(_client_id), _count, _payload.size());

                    break;
                }
            }

            const auto _status = get_status(_count > 0);

            next(request, _status, {
                     {"count", _count}
                 });
        }
    }
}
