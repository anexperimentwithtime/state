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

namespace aewt {
    validator::validator(boost::json::object data) {
        if (!data.contains("transaction_id")) {
            bag_.insert_or_assign("transaction_id", "transaction_id attribute must be present");
            passed_ = false;
        } else {
            if (!data.at("transaction_id").is_string()) {
                bag_.insert_or_assign("transaction_id", "transaction_id attribute must be string");
                passed_ = false;
            } else {
                if (!is_uuid(data.at("transaction_id").as_string().c_str())) {
                    bag_.insert_or_assign("transaction_id", "transaction_id attribute must be uuid");
                    passed_ = false;
                } else {
                    if (!data.contains("action")) {
                        bag_.insert_or_assign("action", "action attribute must be present");
                        passed_ = false;
                    } else {
                        if (!data.at("action").is_string()) {
                            bag_.insert_or_assign("action", "action attribute must be string");
                            passed_ = false;
                        } else {
                            passed_ = true;
                        }
                    }
                }
            }
        }
    }

    bool validator::get_passed() const { return passed_; }

    std::map<std::string, std::string> validator::get_bag() const { return bag_; }

    bool validator::is_uuid(const char * uuid) {
        try {
            constexpr boost::uuids::string_generator _generator;
            const boost::optional<boost::uuids::uuid> _uuid = _generator(uuid);
            return true;
        } catch (...) {
            return false;
        }
    }
} // namespace aewt
