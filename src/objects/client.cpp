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

#include <aewt/client.hpp>

#include <aewt/logger.hpp>
#include <boost/core/ignore_unused.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace aewt {
    client::client(const boost::uuids::uuid id, const boost::uuids::uuid session_id, const bool is_local)  : id_(id), session_id_(session_id), is_local_(is_local) {
        LOG_INFO("client {} allocated", to_string(id_));
    }

    client::~client() { LOG_INFO("client {} released", to_string(id_)); }

    boost::uuids::uuid client::get_id() const { return id_; }

    boost::uuids::uuid client::get_session_id() const {
        return session_id_;
    }

    std::optional<boost::asio::ip::tcp::socket>& client::get_socket() { return socket_; }

    void client::send(std::shared_ptr<boost::json::object> data) {
        boost::ignore_unused(data);

        if (socket_.has_value()) {
            if (const auto & _socket = socket_.value(); _socket.is_open()) {

            }
        }
    }
} // namespace aewt
