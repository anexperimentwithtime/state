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

#include <aewt/validators/send.hpp>

#include <aewt/response.hpp>
#include <aewt/validator.hpp>

namespace aewt::validators {
    bool send(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response,
                   const boost::json::object &data) {
        if (!data.contains("params")) {
            response->mark_as_failed(transaction_id, "unprocessable entity",
                                     {{"params", "params attribute must be present"}});
            return false;
        }

        const boost::json::value _params = data.at("params");
        if (!_params.is_object()) {
            response->mark_as_failed(transaction_id, "unprocessable entity",
                                     {{"params", "params attribute must be object"}});
            return false;
        }

        const boost::json::object _params_object = _params.as_object();
        if (!_params_object.contains("sender_id")) {
            response->mark_as_failed(transaction_id, "unprocessable entity",
                                     {{"params", "params sender_id attribute must be present"}});
            return false;
        }

        const boost::json::value _sender_id = _params_object.at("sender_id");
        if (!_sender_id.is_string()) {
            response->mark_as_failed(transaction_id, "unprocessable entity",
                                     {{"params", "params sender_id attribute must be string"}});
            return false;
        }

        if (!validator::is_uuid(_sender_id.as_string().c_str())) {
            response->mark_as_failed(transaction_id, "unprocessable entity",
                                     {{"params", "params sender_id attribute must be uuid"}});
            return false;
        }

        if (!_params_object.contains("receiver_id")) {
            response->mark_as_failed(transaction_id, "unprocessable entity",
                                     {{"params", "params receiver_id attribute must be present"}});
            return false;
        }

        const boost::json::value _receiver_id = _params_object.at("receiver_id");
        if (!_receiver_id.is_string()) {
            response->mark_as_failed(transaction_id, "unprocessable entity",
                                     {{"params", "params receiver_id attribute must be string"}});
            return false;
        }

        if (!validator::is_uuid(_receiver_id.as_string().c_str())) {
            response->mark_as_failed(transaction_id, "unprocessable entity",
                                     {{"params", "params receiver_id attribute must be uuid"}});
            return false;
        }

        if (!_params_object.contains("session_id")) {
            response->mark_as_failed(transaction_id, "unprocessable entity",
                                     {{"params", "params session_id attribute must be present"}});
            return false;
        }

        const boost::json::value _session_id = _params_object.at("session_id");
        if (!_session_id.is_string()) {
            response->mark_as_failed(transaction_id, "unprocessable entity",
                                     {{"params", "params session_id attribute must be string"}});
            return false;
        }

        if (!validator::is_uuid(_session_id.as_string().c_str())) {
            response->mark_as_failed(transaction_id, "unprocessable entity",
                                     {{"params", "params session_id attribute must be uuid"}});
            return false;
        }

        if (!_params_object.contains("payload")) {
            response->mark_as_failed(transaction_id, "unprocessable entity",
                                     {{"params", "params payload attribute must be present"}});
            return false;
        }

        const boost::json::value _payload = _params_object.at("payload");
        if (!_payload.is_object()) {
            response->mark_as_failed(transaction_id, "unprocessable entity",
                                     {{"params", "params payload attribute must be object"}});
            return false;
        }

        return true;
    }
}
