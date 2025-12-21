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

#include <aewt/session.hpp>

#include <aewt/logger.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace aewt {
    session::session(const boost::uuids::uuid id,
                     boost::asio::ip::tcp::socket socket)
        : id_(id), socket_(std::move(socket)) {
        LOG_INFO("session {} allocated", to_string(id_));
    }

    session::~session() { LOG_INFO("session {} released", to_string(id_)); }

    boost::uuids::uuid session::get_id() const { return id_; }

    boost::asio::ip::tcp::socket &session::get_socket() { return socket_; }

    void session::send(std::shared_ptr<boost::json::object> data) {
        if (socket_.is_open()) {}
    }
} // namespace aewt
