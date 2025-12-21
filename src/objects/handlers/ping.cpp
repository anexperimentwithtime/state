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

#include <aewt/handlers/ping.hpp>

#include <aewt/response.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace aewt::handlers {
    void ping(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response) {
        response->set_data(transaction_id, "pong", {
                               {"transaction_id", to_string(transaction_id)},
                               {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()}
                           });
    }
}
