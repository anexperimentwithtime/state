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

#include <aewt/validators/register_validator.hpp>

#include <aewt/validators/id_validator.hpp>

#include <aewt/request.hpp>
#include <aewt/validator.hpp>

#include <aewt/utils.hpp>

namespace aewt::validators {
    bool session_validator(const request &request) {
        const boost::json::value &_params = get_params_as_value(request);
        const boost::json::object &_params_object = _params.as_object();
        if (!_params_object.contains("host")) {
            mark_as_invalid(request, "params", "params host attribute must be present");
            return false;
        }

        if (const boost::json::value &_host = _params_object.at("host"); !_host.is_string()) {
            mark_as_invalid(request, "params", "params host attribute must be string");
            return false;
        }

        if (!_params_object.contains("sessions_port")) {
            mark_as_invalid(request, "params", "params sessions_port attribute must be present");
            return false;
        }

        if (const boost::json::value &_sessions_port = _params_object.at("sessions_port"); !_sessions_port.
            is_number()) {
            mark_as_invalid(request, "params", "params sessions_port attribute must be number");
            return false;
        }

        if (!_params_object.contains("clients_port")) {
            mark_as_invalid(request, "params", "params clients_port attribute must be present");
            return false;
        }

        if (const boost::json::value &_clients_port = _params_object.at("clients_port"); !_clients_port.is_number()) {
            mark_as_invalid(request, "params", "params clients_port attribute must be number");
            return false;
        }

        return true;
    }
}
