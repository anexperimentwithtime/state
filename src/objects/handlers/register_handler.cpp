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

#include <aewt/logger.hpp>
#include <aewt/handlers/register_handler.hpp>

#include <aewt/state.hpp>
#include <aewt/request.hpp>
#include <aewt/session.hpp>

#include <aewt/validators/register_validator.hpp>

#include <aewt/utils.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace aewt::handlers {
    void register_handler(const request &request) {
        auto &_state = request.state_;

        switch (request.context_) {
            case on_client: {
                next(request, "no effect");
                break;
            }
            case on_session: {
                if (validators::register_validator(request)) {
                    const auto &_params = get_params(request);
                    const auto _sessions_port = get_param_as_number(_params, "sessions_port");
                    const auto _clients_port = get_param_as_number(_params, "clients_port");
                    const auto _registered = get_param_as_bool(_params, "registered");

                    if (const auto _session = _state->get_session(request.entity_id_); _session.has_value()) {
                        const auto &_instance = _session.value();
                        _instance->set_clients_port(_clients_port);
                        _instance->set_sessions_port(_sessions_port);
                        _instance->mark_as_registered();

                        LOG_INFO("state_id=[{}] action=[register] context=[{}] session_id=[{}] sessions_port=[{}] clients_port=[{}] registered=[{}] status=[ok]", to_string(request.state_->get_id()), kernel_context_to_string(request.context_), to_string(request.entity_id_), _sessions_port, _clients_port, _registered);

                        _state->sync(_instance, _registered);

                        next(request, "ok");
                    } else {
                        next(request, "no effect");

                        LOG_INFO("state_id=[{}] action=[register] context=[{}] status=[no effect]", to_string(request.state_->get_id()), kernel_context_to_string(request.context_));
                    }
                }
                break;
            }
        }
    }
}
