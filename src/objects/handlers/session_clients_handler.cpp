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

#include <aewt/handlers/session_clients_handler.hpp>

#include <aewt/state.hpp>
#include <aewt/request.hpp>

#include <aewt/validators/session_id_validator.hpp>

#include <aewt/utils.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace aewt::handlers {
    void session_clients_handler(const request &request) {
        const auto &_state = request.state_;
        if (validators::session_id_validator(request)) {
            const auto &_params = get_params(request);
            const auto &_session_id = get_param_as_id(_params, "session_id");
            const auto _clients = _state->get_clients_by_session(_session_id);

            const boost::json::object _data = {
                {"id", to_string(_session_id)},
                {"clients", make_array_of_ids(_clients)},
            };

            next(request, "ok", _data);
        }
    }
}
