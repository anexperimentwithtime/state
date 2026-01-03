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

#ifndef AEWT_CONFIG_HPP
#define AEWT_CONFIG_HPP

#include <string>
#include <atomic>

namespace aewt {
    struct config {
        /**
         * Address
         */
        std::string address_ = "0.0.0.0";

        /**
         * Sessions Port
         */
        unsigned short sessions_port_ = 11000;

        /**
         * Clients Port
         */
        unsigned short clients_port_ = 12000;

        /**
         * Is Node
         */
        bool is_node_ = false;

        /**
         * Remote Address
         */
        std::string remote_address_ = "127.0.0.1";

        /**
         * Remote Sessions Port
         */
        unsigned short remote_sessions_port_ = 9000;

        /**
         * Remote Clients Port
         */
        unsigned short remote_clients_port_ = 10000;

        /**
         * Threads
         */
        unsigned short threads_ = 1;

        /**
         * Registered
         */
        std::atomic<bool> registered_ = false;

        /**
         * REPL Enabled
         */
        bool repl_enabled = true;
    };
} // namespace aewt

#endif  // AEWT_CONFIG_HPP
