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

#include <aewt/utils.hpp>

#include <aewt/request.hpp>
#include <aewt/response.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace aewt {
    void next(const request &request, const char *status, const boost::json::object &data) {
        request.response_->set_data(
            request.transaction_id_,
            status,
            request.timestamp_,
            data);
    }

    void mark_as_invalid(const request &request, const char *field, const char *argument) {
        request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_, {{field, argument}});
    }

    std::string get_param_as_string(const boost::json::object &params, const char *field) {
        return std::string{ params.at(field).as_string() };
    }

    const boost::json::object &get_params(const request &request) {
        return request.data_.at("params").as_object();
    }

    const boost::json::object &get_param_as_object(const boost::json::object &params, const char *field) {
        return params.at(field).as_object();
    }

    const boost::json::value &get_params_as_value(const request &request) {
        return request.data_.at("params");
    }

    boost::uuids::uuid get_param_as_id(const boost::json::object &params, const char *field) {
        return boost::lexical_cast<boost::uuids::uuid>(std::string{ params.at(field).as_string() });
    }
} // namespace aewt
