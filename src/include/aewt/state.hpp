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

#ifndef AEWT_STATE_HPP
#define AEWT_STATE_HPP

#include <boost/functional/hash.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <chrono>
#include <map>
#include <memory>
#include <shared_mutex>

namespace aewt {
    /**
     * Forward Session
     */
    class session;

    /**
     * Instance
     */
    class state : public std::enable_shared_from_this<state> {
    public:
        /**
         * Constructor
         */
        state();

        /**
         * Destructor
         */
        ~state();

        /**
         * Get Generator
         *
         * @return random_generator
         */
        boost::uuids::random_generator get_generator() const;

        /**
         * Get ID
         *
         * @return uuid
         */
        boost::uuids::uuid get_id() const;

        /**
         * Get Created At
         *
         * @return system_clock::time_point
         */
        std::chrono::system_clock::time_point get_created_at() const;

        /**
         * Get Sessions
         *
         * @return vector<shared_ptr<session>>
         */
        std::vector<std::shared_ptr<session> > get_sessions() const;

        /**
         * Get Session
         *
         * @param id uuid
         * @return optional<shared_ptr<session>>
         */
        std::optional<std::shared_ptr<session> > get_session(
            boost::uuids::uuid id) const;

        /**
         * Add Session
         *
         * @param session shared_ptr<session>
         */
        void add_session(std::shared_ptr<session> session);

        /**
         * Remove Session
         *
         * @param id uuid
         */
        void remove_session(boost::uuids::uuid id);

    private:
        /**
         * Generator
         */
        boost::uuids::random_generator generator_;

        /**
         * ID
         */
        boost::uuids::uuid id_;

        /**
         * Created At
         */
        std::chrono::system_clock::time_point created_at_;

        /**
         * Sessions
         */
        std::map<boost::uuids::uuid, std::shared_ptr<session> > sessions_;

        /**
         * Sessions Shared Mutex
         */
        mutable std::shared_mutex sessions_mutex_;
    };
} // namespace aewt

#endif  // AEWT_STATE_HPP
