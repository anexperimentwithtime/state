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

#include <aewt/validators/id_validator.hpp>

#include <aewt/request.hpp>
#include <aewt/validator.hpp>

#include <aewt/utils.hpp>
#include <fmt/format.h>

namespace aewt::validators {
    bool id_validator(const request &request, const boost::json::object &params, const char * attribute) {
        if (!params.contains(attribute)) {
            mark_as_invalid(request, "params", fmt::format("params {} attribute must be present", attribute).data());
            return false;
        }

        const boost::json::value &_value = params.at(attribute);
        if (!_value.is_string()) {
            mark_as_invalid(request, "params", fmt::format("params {} attribute must be string", attribute).data());
            return false;
        }

        if (!validator::is_uuid(_value.as_string().c_str())) {
            mark_as_invalid(request, "params", fmt::format("params {} attribute must be uuid", attribute).data());
            return false;
        }

        return true;
    }
}
