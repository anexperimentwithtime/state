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

#include <aewt/handlers/session_client_exists_handler.hpp>

#include <aewt/state.hpp>
#include <aewt/request.hpp>

#include <aewt/utils.hpp>

namespace aewt::handlers {
    void session_client_exists_handler(const request &request) {
        const auto &_state = request.state_;
        const auto &_params = get_params(request);
        const auto _exists = _state->get_client_exists_on_session(request.client_id_, request.session_id_);
        const auto _status = get_status(_exists,"yes", "no");
        next(request, _status);
    }
}
