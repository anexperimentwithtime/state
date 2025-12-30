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

#include <aewt/handlers/client_leave_handler.hpp>

#include <aewt/state.hpp>
#include <aewt/request.hpp>

#include <aewt/utils.hpp>
#include <aewt/distribute.hpp>

namespace aewt::handlers {
    void client_leave_handler(const request &request) {
        switch (request.context_) {
            case on_client:
                next(request, "no effect");
                break;
            case on_session:
                const auto &_params = get_params(request);
                auto _id = get_param_as_id(_params, "id");
                const auto _removed = request.state_->remove_client(_id);
                const auto _status = get_status(_removed);
                next(request, _status);
                break;
        }
    }
}
