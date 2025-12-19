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

#ifndef AEWT_KERNEL_HPP
#define AEWT_KERNEL_HPP

#include <boost/json/object.hpp>
#include <memory>

namespace aewt {
    /**
     * Forward State
     */
    class state;

    /**
     * Forward Session
     */
    class session;

    /**
     * Forward Response
     */
    class response;

    /**
     * Kernel
     *
     * @param state
     * @param session
     * @param data
     * @return shared_ptr<response>
     */
    std::shared_ptr<response> kernel(const std::shared_ptr<state>& state,
                                     const std::shared_ptr<session>& session,
                                     boost::json::object data);
} // namespace aewt

#endif  // AEWT_KERNEL_HPP
