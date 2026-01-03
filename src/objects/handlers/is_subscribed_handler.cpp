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

#include <aewt/state.hpp>
#include <aewt/request.hpp>
#include <aewt/kernel_context.hpp>

#include <aewt/validators/is_subscribed_validator.hpp>

#include <aewt/utils.hpp>
#include <aewt/logger.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace aewt::handlers {
    void is_subscribed_handler(const request &request) {
        const auto &_state = request.state_;
        if (validators::is_subscribed_validator(request)) {
            const auto &_params = get_params(request);
            const auto _channel = get_param_as_string(_params, "channel");

            switch (request.context_) {
                case on_client: {
                    const auto _success = request.state_->is_subscribed(request.entity_id_, _channel);

                    const auto _status = get_status(_success, "yes", "no");

                    LOG_INFO("state_id=[{}] action=[is_subscribed] context=[{}] client_id=[{}] status=[{}]",
                             to_string(_state->get_id()), kernel_context_to_string(request.context_),
                             to_string(request.entity_id_), _status);

                    next(request, _status);
                    break;
                }
                case on_session: {
                    next(request, "no effect");

                    LOG_INFO("state_id=[{}] action=[is_subscribed] context=[{}] session_id=[{}] status=[{}]",
                             to_string(_state->get_id()), kernel_context_to_string(request.context_),
                             to_string(request.entity_id_), "no effect");

                    break;
                }
            }
        }
    }
}
