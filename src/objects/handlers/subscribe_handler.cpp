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

#include <aewt/response.hpp>
#include <aewt/state.hpp>
#include <aewt/session.hpp>
#include <aewt/request.hpp>

#include <aewt/validators/subscriptions_validator.hpp>

#include <aewt/utils.hpp>

namespace aewt::handlers {
    void subscribe_handler(const request &request) {
        if (validators::subscriptions_validator(request)) {
            auto _params = request.data.at("params").as_object();
            const auto _client_id = GET_PARAM_AS_ID(_params, "client_id");
            const std::string _channel{_params.at("channel").as_string()};

            const bool _success = request.state->subscribe(request.session->get_id(), _client_id, _channel);

            const auto _status = _success ? "ok" : "no effect";

            request.response->set_data(request.transaction_id, _status, request.timestamp);
        }
    }
}
