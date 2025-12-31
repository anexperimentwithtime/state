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

#ifndef AEWT_SESSION_HPP
#define AEWT_SESSION_HPP

#include <aewt/session_context.hpp>

#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json/object.hpp>
#include <boost/uuid/uuid.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/uuid/random_generator.hpp>

namespace aewt {
    /**
     * Forward State
     */
    class state;

    /**
     * Session
     */
    class session : public std::enable_shared_from_this<session> {
        /**
         * Sessions port
         */
        unsigned short sessions_port_ = 0;

        /**
         * Clients port
         */
        unsigned short clients_port_ = 0;

        /**
         * Host
         */
        std::string host_;

        /**
         * Registered
         */
        std::atomic<bool> registered_ = false;
    public:
        /**
         * Constructor
         *
         * @param id
         * @param state
         * @param socket
         */
        session(const std::shared_ptr<state> &state, boost::asio::ip::tcp::socket &&socket, boost::uuids::uuid id = boost::uuids::random_generator()());

        /**
         * Destructor
         */
        ~session();

        /**
         * Get ID
         *
         * @return uuid
         */
        boost::uuids::uuid get_id() const;

        /**
         * Get Socket
         *
         * @return tcp::socket
         */
        boost::beast::websocket::stream<boost::beast::tcp_stream> &get_socket();

        /**
         * Send
         *
         * @param data
         */
        void send(std::shared_ptr<std::string const> const &data);

        /**
         * Run
         */
        void run(session_context context);

        /**
         * Get Sessions Port
         *
         * @return
         */
        unsigned short get_sessions_port() const;

        /**
         * Set Sessions Port
         *
         * @param port
         */
        void set_sessions_port(unsigned short port);

        /**
         * Get Clients Port
         *
         * @return
         */
        unsigned short get_clients_port() const;

        /**
         * Set Clients Port
         *
         * @param port
         */
        void set_clients_port(unsigned short port);

        /**
         * Get Host
         *
         * @return
         */
        std::string get_host() const;

        /**
         * Set Host
         *
         * @param host
         */
        void set_host(const std::string &host);

        /**
         * Mark As Registered
         */
        void mark_as_registered();

        /**
         * Get Registered
         *
         * @return
         */
        bool get_registered() const;
    private:
        /**
         * State
         */
        std::shared_ptr<state> state_;

        /**
         * ID
         */
        boost::uuids::uuid id_;

        /**
         * Socket
         */
        boost::beast::websocket::stream<boost::beast::tcp_stream> socket_;

        /**
         * Buffer
         */
        boost::beast::flat_buffer buffer_;

        /**
         * Queue
         */
        std::vector<std::shared_ptr<std::string const> > queue_;

        /**
         * On Run
         */
        void on_run(session_context context);

        /**
         * On Accept
         *
         * @param context
         * @param ec
         */
        void on_accept(session_context context, const boost::beast::error_code &ec);

        /**
         * Do Read
         */
        void do_read();

        /**
         * On Read
         *
         * @param ec
         * @param bytes_transferred
         */
        void on_read(const boost::system::error_code &ec, std::size_t bytes_transferred);

        /**
         * On Send
         *
         * @param data
         */
        void on_send(std::shared_ptr<std::string const> const &data);

        /**
         * On Write
         *
         * @param ec
         * @param bytes_transferred
         */
        void on_write(const boost::beast::error_code &ec, std::size_t bytes_transferred);
    };
} // namespace aewt

#endif  // AEWT_SESSION_HPP
