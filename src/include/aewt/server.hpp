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

namespace aewt {
    /**
     * Forward State
     */
    class state;

    class server : std::enable_shared_from_this<server> {
        /**
         * Config
         */
        std::shared_ptr<config> config_;

        /**
         * State
         */
        std::shared_ptr<state> state_;

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
    };
} // namespace aewt

#endif  // AEWT_SERVER_HPP
