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
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <boost/uuid/uuid.hpp>

namespace aewt {
    /**
     * Clients By Session
     */
    struct clients_by_session {
    };

    /**
     * Clients By Client
     */
    struct clients_by_client {
    };

    /**
     * Clients By Client and Session
     */
    struct clients_by_client_session {
    };

    using namespace boost::multi_index;

    /**
     * Clients
     */
    using clients = multi_index_container<
        std::shared_ptr<client>,
        indexed_by<
            ordered_non_unique<
                tag<clients_by_session>,
                const_mem_fun<client, boost::uuids::uuid, &client::get_session_id>
            >,
            ordered_unique<
                tag<clients_by_client>,
                const_mem_fun<client, boost::uuids::uuid, &client::get_id>
            >,
            ordered_unique<
                tag<clients_by_client_session>,
                composite_key<
                    std::shared_ptr<client>,
                    const_mem_fun<client, boost::uuids::uuid, &client::get_id>,
                    const_mem_fun<client, boost::uuids::uuid, &client::get_session_id>
                >
            >
        >
    >;
} // namespace aewt

#endif  // AEWT_CLIENTS_HPP
