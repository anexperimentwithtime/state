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

#include <aewt/handlers/ping_handler.hpp>

#include <aewt/request.hpp>

#include <aewt/utils.hpp>
#include <aewt/logger.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace aewt::handlers {
    void ping_handler(const request &request) {
        LOG_INFO("state_id=[{}] action=[ping] context=[{}] status=[{}]", to_string(request.state_->get_id()),
                 kernel_context_to_string(request.context_), "pong");

        next(request, "pong");
    }
}
