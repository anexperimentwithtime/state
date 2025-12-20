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

#ifndef AEWT_SUBSCRIPTIONS_HPP
#define AEWT_SUBSCRIPTIONS_HPP

#include <aewt/subscription.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <boost/uuid/uuid.hpp>

namespace aewt {
    struct subscriptions_by_session {};
    struct subscriptions_by_client  {};
    struct subscriptions_by_channel {};
    struct subscriptions_by_session_client_channel {};

    using subscriptions = boost::multi_index::multi_index_container<
        subscription,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<subscriptions_by_session>,
                boost::multi_index::member<subscription, boost::uuids::uuid, &subscription::session_id_>
            >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<subscriptions_by_client>,
                boost::multi_index::member<subscription, boost::uuids::uuid, &subscription::client_id_>
            >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<subscriptions_by_channel>,
                boost::multi_index::member<subscription, std::string, &subscription::channel_>
            >,
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<subscriptions_by_session_client_channel>,
                boost::multi_index::composite_key<
                    subscription,
                    boost::multi_index::member<subscription, boost::uuids::uuid, &subscription::session_id_>,
                    boost::multi_index::member<subscription, boost::uuids::uuid, &subscription::client_id_>,
                    boost::multi_index::member<subscription, std::string,        &subscription::channel_>
                >
            >
        >
    >;
} // namespace aewt

#endif  // AEWT_SUBSCRIPTIONS_HPP
