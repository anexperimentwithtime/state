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
#include <aewt/session.hpp>
#include <aewt/request.hpp>

#include <aewt/utils.hpp>
#include <aewt/distribute.hpp>

namespace aewt::handlers {
    void client_leave_handler(const request &request) {
        const auto _removed = request.state_->remove_client(request.client_id_);

        auto _count = std::size_t { 0 };

        if (request.is_local_ && _removed)
            _count = distribute_to_others(request.state_, request.data_, request.session_id_);

        const auto _status = get_status(_removed);

        next(request, _status, {
                 {"timestamp", request.timestamp_},
                 {"count", _count}
             });
    }
}
