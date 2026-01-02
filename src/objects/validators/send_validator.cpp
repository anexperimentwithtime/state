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

#include <aewt/validators/send_validator.hpp>

#include <aewt/validators/id_validator.hpp>
#include <aewt/validator.hpp>

#include <aewt/request.hpp>

#include <aewt/utils.hpp>

namespace aewt::validators {
    bool send_validator(const request &request) {
        const boost::json::value &_params = get_params_as_value(request);
        const boost::json::object &_params_object = _params.as_object();

        if (!_params_object.contains("payload")) {
            mark_as_invalid(request, "params", "params payload attribute must be present");
            return false;
        }

        if (const boost::json::value &_payload = _params_object.at("payload"); !_payload.is_object()) {
            mark_as_invalid(request, "params", "params payload attribute must be object");
            return false;
        }

        if (request.context_ == on_session) {
            return id_validator(request, _params_object, "from_client_id") &&
                   id_validator(request, _params_object, "to_client_id");
        }

        return id_validator(request, _params_object, "to_client_id");
    }
}
