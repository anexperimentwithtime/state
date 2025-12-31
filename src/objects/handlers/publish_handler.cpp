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

#include <aewt/handlers/publish_handler.hpp>

#include <aewt/state.hpp>
#include <aewt/session.hpp>
#include <aewt/request.hpp>
#include <aewt/kernel_context.hpp>

#include <aewt/logger.hpp>

#include <aewt/validators/publish_validator.hpp>

#include <aewt/utils.hpp>

namespace aewt::handlers {
    void publish_handler(const request &request) {
        auto &_state = request.state_;

        if (validators::publish_validator(request)) {
            const auto &_params = get_params(request);
            const auto _channel = get_param_as_string(_params, "channel");
            const auto &_payload = get_param_as_object(_params, "payload");
            std::size_t _count = 0;
            switch (request.context_) {
                case on_client: {
                    _count = _state->publish_to_clients(
                        request,
                        _state->get_id(),
                        request.entity_id_,
                        _channel,
                        _payload
                    );

                    const auto _ = request.state_->publish_to_sessions(
                        request,
                        request.entity_id_,
                        _channel,
                        _payload
                    );
                    boost::ignore_unused(_);
                    break;
                }
                case on_session: {
                    const auto &_client_id = get_param_as_id(_params, "client_id");
                    _count = _state->publish_to_clients(
                        request,
                        _state->get_id(),
                        _client_id,
                        _channel,
                        _payload
                    );
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
