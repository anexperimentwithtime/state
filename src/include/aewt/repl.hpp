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

#ifndef AEWT_REPL_HPP
#define AEWT_REPL_HPP

#include <memory>
#include <boost/asio/io_context.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

namespace aewt {
    /**
     * Forward State
     */
    class state;

    /**
     * REPL
     */
    class repl {
    public:
        /**
         * Constructor
         *
         * @param state
         */
        explicit repl(const std::shared_ptr<state> &state);

    private:
        /**
         * Start
         */
        void start();

        /**
         * Read
         *
         * @param error
         * @param bytes_transferred
         */
        void read(const boost::system::error_code &error, std::size_t bytes_transferred);

        /**
         * Write
         *
         * @param error
         */
        void write(const boost::system::error_code &error);

        /**
         * Input
         */
        boost::asio::posix::stream_descriptor input_;

        /**
         * Output
         */
        boost::asio::posix::stream_descriptor output_;

        /**
         * Buffer
         */
        std::string buffer_;

        /**
         * State
         */
        std::shared_ptr<state> state_;
    };
} // namespace aewt

#endif  // AEWT_REPL_HPP
