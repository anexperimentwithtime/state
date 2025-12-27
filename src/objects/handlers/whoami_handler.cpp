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

#include <aewt/handlers/whoami_handler.hpp>

#include <aewt/request.hpp>

#include <aewt/utils.hpp>

namespace aewt::handlers {
    void whoami_handler(const request &request) {
        const auto _session = request.state_->get_session(request.session_id_);
        const auto _data = make_session_object(_session.value());
        next(request, "ok", _data);
    }
}
