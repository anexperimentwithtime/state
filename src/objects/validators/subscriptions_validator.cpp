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

#include <aewt/validators/subscriptions_validator.hpp>

#include <aewt/validators/id_validator.hpp>

#include <aewt/request.hpp>

#include <aewt/utils.hpp>

namespace aewt::validators {
    bool subscriptions_validator(const request &request) {
        const boost::json::value &_params = get_params_as_value(request);
        const boost::json::object &_params_object = _params.as_object();
        if (!_params_object.contains("channel")) {
            mark_as_invalid(request, "params", "params channel attribute must be present");
            return false;
        }

        if (const boost::json::value &_channel = _params_object.at("channel"); !_channel.is_string()) {
            mark_as_invalid(request, "params", "params channel attribute must be string");
            return false;
        }

        if (request.context_ == on_session) {
            return id_validator(request, _params_object, "client_id");
        }

        return true;
    }
}
