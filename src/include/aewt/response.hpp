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

#ifndef AEWT_RESPONSE_HPP
#define AEWT_RESPONSE_HPP

#include <atomic>
#include <boost/json/object.hpp>
#include <boost/uuid/uuid.hpp>
#include <map>
#include <memory>

namespace aewt {
    /**
     * Response
     */
    class response : public std::enable_shared_from_this<response> {
        /**
         * Failed
         */
        std::atomic<bool> failed_ = false;

        /**
         * Processed
         */
        std::atomic<bool> processed_ = false;

        /**
         * Data
         */
        boost::json::object data_;

    public:
        /**
         * Get Failed
         *
         * @return bool
         */
        bool get_failed() const;

        /**
         * Get Processed
         *
         * @return bool
         */
        bool get_processed() const;

        /**
         * Get Data
         *
         * @return json::object
         */
        boost::json::object get_data() const;

        /**
         * Mark As Failed
         *
         * @param transaction_id
         * @param error
         * @param bag
         */
        void mark_as_failed(boost::uuids::uuid transaction_id, const char *error, const std::map<std::string, std::string> &bag);

        /**
         * Mark As Processed
         */
        void mark_as_processed();

        /**
         * Set Data
         * @param transaction_id
         * @param message
         * @param data
         */
        void set_data(boost::uuids::uuid transaction_id, const char *message, const boost::json::object &data = {});
    };
} // namespace aewt

#endif  // AEWT_RESPONSE_HPP
