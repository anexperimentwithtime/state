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

#include <aewt/handlers/leave_handler.hpp>

#include <aewt/validators/id_validator.hpp>

#include <aewt/state.hpp>
#include <aewt/request.hpp>

#include <aewt/utils.hpp>
#include <aewt/logger.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace aewt::handlers {
    void leave_handler(const request &request) {
        auto &_state = request.state_;

        switch (request.context_) {
            case on_client: {
                LOG_INFO("state_id=[{}] action=[leave] context=[{}] client_id=[{}] status=[{}]", to_string(_state->get_id()), kernel_context_to_string(request.context_), to_string(request.entity_id_), "no effect");

                next(request, "no effect");
                break;
            }
            case on_session: {
                if (const auto &_params = get_params(request); validators::id_validator(request, _params, "client_id")) {
                    const auto _client_id = get_param_as_id(_params, "client_id");
                    const auto _removed = _state->remove_client(_client_id);
                    const auto _status = get_status(_removed);

                    LOG_INFO("state_id=[{}] action=[leave] context=[{}] session_id=[{}] client_id=[{}] status=[{}]", to_string(_state->get_id()), kernel_context_to_string(request.context_), to_string(request.entity_id_), to_string(_client_id), _status);
                    next(request, _status);
                }
                break;
            }
        }
    }
}
