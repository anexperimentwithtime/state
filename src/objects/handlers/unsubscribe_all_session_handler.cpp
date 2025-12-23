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

#include <aewt/handlers/unsubscribe_all_session_handler.hpp>

#include <aewt/state.hpp>
#include <aewt/request.hpp>

#include <aewt/validators/unsubscribe_all_session_validator.hpp>

#include <aewt/utils.hpp>

namespace aewt::handlers {
    void unsubscribe_all_session_handler(const request &request) {
        if (validators::unsubscribe_all_session_validator(request)) {
            const auto &_params = get_params(request);
            const auto &_session_id = get_param_as_id(_params, "session_id");

            const std::size_t _count = request.state_->unsubscribe_all_session(_session_id);

            const auto _status = get_status(_count > 0);

            next(request, _status, {{"count", _count}});
        }
    }
}
