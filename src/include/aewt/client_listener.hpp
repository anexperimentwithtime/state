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

#pragma once

#ifndef AEWT_CLIENT_LISTENER_HPP
#define AEWT_CLIENT_LISTENER_HPP

#include <memory>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>

namespace aewt {
    class state;

    class client_listener : public std::enable_shared_from_this<client_listener> {
        boost::asio::io_context &ioc_;
        boost::asio::ip::tcp::acceptor acceptor_;
        std::shared_ptr<state> state_;

    public:
        client_listener(boost::asio::io_context &ioc, const boost::asio::ip::tcp::endpoint &endpoint,
                        const std::shared_ptr<state> &state);

        void on_accept(const boost::beast::error_code &ec, boost::asio::ip::tcp::socket socket);

        void do_accept();

        void start();
    };
} // namespace aewt

#endif  // AEWT_CLIENT_LISTENER_HPP
