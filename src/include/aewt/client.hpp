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

#ifndef AEWT_CLIENT_HPP
#define AEWT_CLIENT_HPP

#include <boost/uuid/uuid.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json/object.hpp>
#include <memory>

namespace aewt {
    /**
     * Client
     */
    class client : public std::enable_shared_from_this<client> {
        /**
         * Client ID
         */
        boost::uuids::uuid id_;

        /**
         * Session ID
         */
        boost::uuids::uuid session_id_;

        /**
         * Is Local
         */
        bool is_local_;

    public:
        /**
         * Constructor
         * @param id
         * @param session_id
         * @param is_local
         */
        client(boost::uuids::uuid id, boost::uuids::uuid session_id, bool is_local = false);


        /**
         * Destructor
         */
        ~client();

        /**
         * Get ID
         *
         * @return uuid
         */
        boost::uuids::uuid get_id() const;

        /**
         * Get Session ID
         *
         * @return uuid
         */
        boost::uuids::uuid get_session_id() const;

        /**
         * Get Socket
         *
         * @return tcp::socket
         */
        std::optional<boost::asio::ip::tcp::socket> &get_socket();

        /**
         * Send
         *
         * @param data
         */
        void send(std::shared_ptr<boost::json::object> data);

        /**
         * Get Is Local
         *
         * @return
         */
        bool get_is_local() const;

    private:
        /**
         * Socket
         */
        std::optional<boost::asio::ip::tcp::socket> socket_;
    };
} // namespace aewt

#endif  // AEWT_CLIENT_HPP
