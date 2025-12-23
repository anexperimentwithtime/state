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

#include <aewt/handlers/unimplemented_handler.hpp>

#include <aewt/response.hpp>
#include <aewt/request.hpp>

namespace aewt::handlers {
    void unimplemented_handler(const request &request) {
        request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_, {
                                     {"action", "action attribute isn't implemented"}
                                 });
    }
}
