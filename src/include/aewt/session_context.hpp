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

#ifndef AEWT_SESSION_MODE_HPP
#define AEWT_SESSION_MODE_HPP

#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json/object.hpp>
#include <boost/uuid/uuid.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/uuid/random_generator.hpp>

namespace aewt {
    enum session_context {
      local,
      remote,
    };
} // namespace aewt

#endif  // AEWT_SESSION_MODE_HPP
