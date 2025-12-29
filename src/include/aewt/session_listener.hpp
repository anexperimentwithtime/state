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

#ifndef AEWT_SESSION_LISTENER_HPP
#define AEWT_SESSION_LISTENER_HPP

#include <memory>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>

namespace aewt {
    /**
     * Forward State
     */
    class state;

    /**
     * Session Listener
     */
    class session_listener : public std::enable_shared_from_this<session_listener> {
        /**
         * IO Context
         */
        boost::asio::io_context &ioc_;

        /**
         * Acceptor
         */
        boost::asio::ip::tcp::acceptor acceptor_;

        /**
         * State
         */
        std::shared_ptr<state> state_;

    public:
        /**
         * Constructor
         *
         * @param ioc
         * @param endpoint
         * @param state
         */
        session_listener(boost::asio::io_context &ioc, const boost::asio::ip::tcp::endpoint &endpoint,
                         const std::shared_ptr<state> &state);

        /**
         * On Accept
         *
         * @param ec
         * @param socket
         */
        void on_accept(const boost::beast::error_code &ec, boost::asio::ip::tcp::socket socket);

        /**
         * Do Accept
         */
        void do_accept();

        /**
         * Start
         */
        void start();
    };
} // namespace aewt

#endif  // AEWT_SESSION_LISTENER_HPP
