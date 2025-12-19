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

#ifndef AEWT_STATE_SESSION_HPP
#define AEWT_STATE_SESSION_HPP

#include <boost/asio/ip/tcp.hpp>
#include <boost/uuid/uuid.hpp>

namespace aewt::state {
/**
 * Session
 */
class session : public std::enable_shared_from_this<session> {
 public:
  /**
   * Constructor
   *
   * @param id
   * @param socket
   */
  session(boost::uuids::uuid id, boost::asio::ip::tcp::socket socket);

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
  boost::asio::ip::tcp::socket& get_socket();

 private:
  /**
   * ID
   */
  boost::uuids::uuid id_;

  /**
   * Socket
   */
  boost::asio::ip::tcp::socket socket_;
};
}  // namespace aewt::state

#endif  // AEWT_STATE_SESSION_HPP
