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

#ifndef AEWT_HANDLERS_SESSION_CLIENTS_HANDLER_HPP
#define AEWT_HANDLERS_SESSION_CLIENTS_HANDLER_HPP

#include <boost/uuid/uuid.hpp>
#include <boost/json/object.hpp>
#include <memory>

namespace aewt {
    /**
     * Forward Response
     */
    class response;

    /**
     * Forward Session
     */
    class session;

    /**
     * Forward State
     */
    class state;

    namespace handlers {
        /**
         * Session Clients Handler
         *
         * @param transaction_id
         * @param response
         * @param state
         * @param session
         * @param data
         * @param timestamp
         */
        void session_clients_handler(boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response,
                                     const std::shared_ptr<state> &state,
                                     const std::shared_ptr<session> &session, const boost::json::object &data,
                                     long timestamp);
    }
} // namespace aewt

#endif  // AEWT_HANDLERS_SESSION_CLIENTS_HPP
