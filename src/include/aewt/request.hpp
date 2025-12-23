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

#ifndef AEWT_REQUEST_HPP
#define AEWT_REQUEST_HPP

#include <boost/uuid/uuid.hpp>
#include <boost/json/object.hpp>
#include <memory>

#include <aewt/response.hpp>
#include <aewt/state.hpp>
#include <aewt/session.hpp>

namespace aewt {
    struct request {
        const boost::uuids::uuid transaction_id;
        std::shared_ptr<aewt::response> &response;
        const std::shared_ptr<aewt::state> &state;
        const std::shared_ptr<aewt::session> &session;
        boost::json::object &data;
        long timestamp;
    };
} // namespace aewt

#endif  // AEWT_REQUEST_HPP
