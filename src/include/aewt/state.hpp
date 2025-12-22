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

#include <aewt/subscriptions.hpp>
#include <aewt/clients.hpp>

#include <boost/functional/hash.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <chrono>
#include <map>
#include <memory>
#include <vector>
#include <shared_mutex>
#include <boost/json/object.hpp>

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
         * Get Clients By Session
         *
         * @param session_id
         * @return vector<uuid>
         */
        std::vector<boost::uuids::uuid> get_clients_by_session(boost::uuids::uuid session_id) const;

        /**
         * Get Clients
         *
         * @return vector<uuid>
         */
        std::vector<boost::uuids::uuid> get_clients() const;

        /**
         * Get Client Exists On Session
         *
         * @param client_id
         * @param session_id
         * @return bool
         */
        bool get_client_exists_on_session(boost::uuids::uuid client_id, boost::uuids::uuid session_id) const;

        /**
         * Get Client Exists
         *
         * @param client_id
         * @return bool
         */
        bool get_client_exists(boost::uuids::uuid client_id) const;

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
         * @return bool
         */
        bool add_session(std::shared_ptr<session> session);

        /**
         * Remove Session
         *
         * @param id uuid
         */
        bool remove_session(boost::uuids::uuid id);


        /**
         * Add Client
         *
         * @param client_id
         * @param session_id
         * @return
         */
        bool add_client(boost::uuids::uuid client_id, boost::uuids::uuid session_id);

        /**
         * Remove Client
         *
         * @param client_id
         * @return bool
         */
        bool remove_client(boost::uuids::uuid client_id);

        /**
         * Subscribe
         *
         * @param session_id
         * @param client_id
         * @param channel
         * @return bool
         */
        bool subscribe(const boost::uuids::uuid &session_id, const boost::uuids::uuid &client_id,
                       const std::string &channel);

        /**
         * Unsubscribe
         *
         * @param session_id
         * @param client_id
         * @param channel
         * @return bool
         */
        bool unsubscribe(const boost::uuids::uuid &session_id, const boost::uuids::uuid &client_id,
                         const std::string &channel);

        /**
         * Is Subscribed
         *
         * @param session_id
         * @param client_id
         * @param channel
         * @return bool
         */
        bool is_subscribed(const boost::uuids::uuid &session_id, const boost::uuids::uuid &client_id,
                           const std::string &channel);

        /**
         * Unsubscribe All Client
         *
         * @param client_id
         * @return
         */
        std::size_t unsubscribe_all_client(const boost::uuids::uuid &client_id);

        /**
         * Unsubscribe All Session
         *
         * @param session_id
         * @return
         */
        std::size_t unsubscribe_all_session(const boost::uuids::uuid &session_id);

        /**
         * Broadcast
         *
         * @param transaction_id
         * @param session_id
         * @param client_id
         * @param data
         *
         * @return size_t
         */
        std::size_t broadcast(boost::uuids::uuid transaction_id, boost::uuids::uuid session_id,
                              boost::uuids::uuid client_id, boost::json::object data);

        /**
         * Publish
         *
         * @param transaction_id
         * @param session_id
         * @param client_id
         * @param channel
         * @param data
         *
         * @return size_t
         */
        std::size_t publish(boost::uuids::uuid transaction_id, boost::uuids::uuid session_id,
                            boost::uuids::uuid client_id, const std::string &channel, boost::json::object data);

        /**
         * Get Session
         *
         * @param client_id
         *
         * @return vector<subscription>
         */
        std::vector<subscription> get_client_subscriptions(boost::uuids::uuid client_id);

        /**
         * Send
         *
         * @param transaction_id
         * @param session_id
         * @param sender_id
         * @param receiver_id
         * @param data
         * @return bool
         */
        bool send(boost::uuids::uuid transaction_id, boost::uuids::uuid session_id, boost::uuids::uuid sender_id,
                  boost::uuids::uuid receiver_id, const boost::json::object &data) const;

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

        /**
         * Clients
         */
        clients clients_;

        /**
         * Clients Shared Mutex
         */
        mutable std::shared_mutex clients_mutex_;

        /**
         * Subscriptions
         */
        subscriptions subscriptions_;

        /**
         * Sessions Shared Mutex
         */
        mutable std::shared_mutex subscriptions_mutex_;
    };
} // namespace aewt

#endif  // AEWT_STATE_HPP
