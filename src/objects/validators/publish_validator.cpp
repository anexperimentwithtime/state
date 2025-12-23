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

#include <aewt/validators/publish_validator.hpp>

#include <aewt/response.hpp>
#include <aewt/request.hpp>
#include <aewt/validator.hpp>

namespace aewt::validators {
    bool publish_validator(const request &request) {
        if (!request.data_.contains("params")) {
            request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                             {{"params", "params attribute must be present"}});
            return false;
        }

        const boost::json::value _params = request.data_.at("params");
        if (!_params.is_object()) {
            request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                             {{"params", "params attribute must be object"}});
            return false;
        }

        const boost::json::object _params_object = _params.as_object();
        if (!_params_object.contains("channel")) {
            request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                             {{"params", "params channel attribute must be present"}});
            return false;
        }

        if (const boost::json::value _channel = _params_object.at("channel"); !_channel.is_string()) {
            request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                             {{"params", "params channel attribute must be string"}});
            return false;
        }

        if (!_params_object.contains("client_id")) {
            request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                             {{"params", "params client_id attribute must be present"}});
            return false;
        }

        const boost::json::value _client_id = _params_object.at("client_id");
        if (!_client_id.is_string()) {
            request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                             {{"params", "params client_id attribute must be string"}});
            return false;
        }

        if (!validator::is_uuid(_client_id.as_string().c_str())) {
            request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                             {{"params", "params client_id attribute must be uuid"}});
            return false;
        }

        if (!_params_object.contains("session_id")) {
            request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                             {{"params", "params session_id attribute must be present"}});
            return false;
        }

        const boost::json::value _session_id = _params_object.at("session_id");
        if (!_session_id.is_string()) {
            request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                             {{"params", "params session_id attribute must be string"}});
            return false;
        }

        if (!validator::is_uuid(_session_id.as_string().c_str())) {
            request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                             {{"params", "params session_id attribute must be uuid"}});
            return false;
        }

        if (!_params_object.contains("payload")) {
            request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                             {{"params", "params payload attribute must be present"}});
            return false;
        }

        const boost::json::value _payload = _params_object.at("payload");
        if (!_payload.is_object()) {
            request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                             {{"params", "params payload attribute must be object"}});
            return false;
        }

        return true;
    }
}
