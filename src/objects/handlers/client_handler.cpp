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

#include <aewt/handlers/client_handler.hpp>

#include <aewt/validators/client_id_validator.hpp>

#include <aewt/state.hpp>
#include <aewt/request.hpp>

#include <aewt/utils.hpp>

namespace aewt::handlers {
    void client_handler(const request &request) {
        if (validators::client_id_validator(request)) {
            const auto &_params = get_params(request);
            const auto &_client_id = get_param_as_id(_params, "client_id");

            if (const auto _client_optional = request.state_->get_client(_client_id); _client_optional.has_value()) {
                const auto& _client = _client_optional.value();

                const boost::json::array _subscriptions = make_channels_array_of_subscriptions(
                    request.state_->get_client_subscriptions(_client_id)
                );

                const boost::json::object _data = make_client_object(_client, _subscriptions);

                next(request, "ok", _data);
            } else {
                next(request, "no effect");
            }
        }
    }
}
