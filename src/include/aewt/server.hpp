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

#ifndef AEWT_SERVER_HPP
#define AEWT_SERVER_HPP

#include <aewt/config.hpp>

#include <memory>
#include <thread>
#include <vector>

namespace aewt {
    /**
     * Forward State
     */
    class state;

    /**
     * Forward Session Listener
     */
    class session_listener;

    /**
     * Forward Client Listener
     */
    class client_listener;

    class server : public std::enable_shared_from_this<server> {
        /**
         * Config
         */
        std::shared_ptr<config> config_;

        /**
         * State
         */
        std::shared_ptr<state> state_;

        /**
         * Session Listener
         */
        std::shared_ptr<session_listener> session_listener_;

        /**
         * Client Listener
         */
        std::shared_ptr<client_listener> client_listener_;

        /**
         * Vector Of Threads
         */
        std::vector<std::jthread> vector_of_threads_;
    public:
        /**
         * Constructor
         *
         * @param configuration
         */
        explicit server(const std::shared_ptr<config> &configuration = std::make_shared<config>());

        /**
         * Start
         */
        void start();

        /**
         * Get Config
         *
         * @return
         */
        std::shared_ptr<config> get_config();

        /**
         * Get State
         *
         * @return
         */
        std::shared_ptr<state> get_state();

        /**
         * Stop
         */
        void stop() const;
    };
} // namespace aewt

#endif  // AEWT_SERVER_HPP
