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

#include <aewt/validator.hpp>

#include <boost/uuid/string_generator.hpp>
#include <boost/optional.hpp>

#include <aewt/utils.hpp>

namespace aewt {
    validator::validator(boost::json::object data) {
        if (!data.contains("action")) {
            bag_.insert_or_assign("action", "action attribute must be present");
            passed_ = false;
            return;
        }

        if (!data.at("action").is_string()) {
            bag_.insert_or_assign("action", "action attribute must be string");
            passed_ = false;
            return;
        }

        if (!data.contains("transaction_id")) {
            bag_.insert_or_assign("transaction_id", "transaction_id attribute must be present");
            passed_ = false;
            return;
        }

        auto _transaction_id_value = data.at("transaction_id");
        if (!_transaction_id_value.is_string()) {
            bag_.insert_or_assign("transaction_id", "transaction_id attribute must be string");
            passed_ = false;
            return;
        }

        const auto _transaction_id = _transaction_id_value.as_string();
        if (!is_uuid(_transaction_id.c_str())) {
            bag_.insert_or_assign("transaction_id", "transaction_id attribute must be uuid");
            passed_ = false;
            return;
        }

        if (!data.contains("params")) {
            bag_.insert_or_assign("params", "params attribute must be present");
            passed_ = false;
            return;
        }

        const boost::json::value &_params = data.at("params");
        if (!_params.is_object()) {
            bag_.insert_or_assign("params", "params attribute must be object");
            passed_ = false;
            return;
        }

        const boost::json::object &_params_object = _params.as_object();
        if (!_params_object.contains("session_id")) {
            bag_.insert_or_assign("params", "params session_id attribute must be present");
            passed_ = false;
            return;
        }

        auto _session_id_value = _params_object.at("session_id");
        if (!_session_id_value.is_string()) {
            bag_.insert_or_assign("params", "params session_id attribute must be string");
            passed_ = false;
            return;
        }

        const auto _session_id = _session_id_value.as_string();
        if (!is_uuid(_session_id.c_str())) {
            bag_.insert_or_assign("params", "params session_id attribute must be uuid");
            passed_ = false;
            return;
        }

        if (!_params_object.contains("client_id")) {
            bag_.insert_or_assign("params", "params client_id attribute must be present");
            passed_ = false;
            return;
        }

        auto _client_id_value = _params_object.at("client_id");
        if (!_client_id_value.is_string()) {
            bag_.insert_or_assign("params", "params client_id attribute must be string");
            passed_ = false;
            return;
        }

        const auto _client_id = _client_id_value.as_string();
        if (!is_uuid(_client_id.c_str())) {
            bag_.insert_or_assign("params", "params client_id attribute must be uuid");
            passed_ = false;
            return;
        }

        passed_ = true;
    }

    bool validator::get_passed() const { return passed_; }

    std::map<std::string, std::string> validator::get_bag() const { return bag_; }

    bool validator::is_uuid(const char *uuid) {
        try {
            constexpr boost::uuids::string_generator _generator;
            const boost::optional<boost::uuids::uuid> _uuid = _generator(uuid);
            return true;
        } catch (...) {
            return false;
        }
    }
} // namespace aewt
