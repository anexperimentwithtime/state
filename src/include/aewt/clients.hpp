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

#ifndef AEWT_CLIENTS_HPP
#define AEWT_CLIENTS_HPP

#include <aewt/client.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <boost/uuid/uuid.hpp>

namespace aewt {
    /**
     * Clients By Session
     */
    struct clients_by_session {};

    /**
     * Clients By Client
     */
    struct clients_by_client  {};

    /**
     * Clients By Client and Session
     */
    struct clients_by_client_session {};

    /**
     * Clients
     */
    using clients = boost::multi_index::multi_index_container<
        client,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<clients_by_session>,
                boost::multi_index::member<client, boost::uuids::uuid, &client::session_id_>
            >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<clients_by_client>,
                boost::multi_index::member<client, boost::uuids::uuid, &client::client_id_>
            >,
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<clients_by_client_session>,
                boost::multi_index::composite_key<
                    client,
                    boost::multi_index::member<client, boost::uuids::uuid, &client::client_id_>,
                    boost::multi_index::member<client, boost::uuids::uuid, &client::session_id_>
                >
            >
        >
    >;
} // namespace aewt

#endif  // AEWT_CLIENTS_HPP
