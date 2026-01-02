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

#include <aewt/handlers/subscribe_handler.hpp>

#include <aewt/state.hpp>
#include <aewt/request.hpp>

#include <aewt/validators/subscriptions_validator.hpp>

#include <aewt/utils.hpp>
#include <aewt/logger.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace aewt::handlers {
    void subscribe_handler(const request &request) {
        auto &_state = request.state_;

        if (validators::subscriptions_validator(request)) {
            const auto &_params = get_params(request);
            const auto _channel = get_param_as_string(_params, "channel");

            switch (request.context_) {
                case on_client: {
                    const bool _success = _state->subscribe(_state->get_id(), request.entity_id_, _channel);
                    const auto _status = get_status(_success);
                    next(request, _status);

                    auto _ = _state->subscribe_to_sessions(request, request.entity_id_, _channel);
                    boost::ignore_unused(_);
                    LOG_INFO("state_id=[{}] action=[subscribe] context=[{}] client_id=[{}] channel=[{}] status=[{}]", to_string(_state->get_id()), kernel_context_to_string(request.context_), to_string(request.entity_id_), _channel, _status);

                }
                break;
                case on_session: {
                    const auto &_client_id = get_param_as_id(_params, "client_id");
                    const bool _success = _state->subscribe(request.entity_id_, _client_id, _channel);
                    const auto _status = get_status(_success);

                    LOG_INFO("state_id=[{}] action=[subscribe] context=[{}] session_id=[{}] client_id=[{}] channel=[{}] status=[{}]", to_string(_state->get_id()), kernel_context_to_string(request.context_), to_string(request.entity_id_), to_string(_client_id), _channel, _status);
                    next(request, _status);
                }
                break;
            }
        }
    }
}
