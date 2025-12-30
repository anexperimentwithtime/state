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

#include <aewt/response.hpp>
#include <map>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace aewt {
    bool response::get_failed() const {
        return failed_.load(std::memory_order_acquire);
    }

    bool response::get_processed() const {
        return processed_.load(std::memory_order_acquire);
    }

    bool response::is_ack() const {
        return is_ack_.load(std::memory_order_acquire);
    }

    void response::mark_as_ack() {
        is_ack_.store(true, std::memory_order_release);
    }

    boost::json::object response::get_data() const { return data_; }

    void response::mark_as_failed(const boost::uuids::uuid transaction_id, const char *error, long timestamp,
                                  const std::map<std::string, std::string> &bag) {
        failed_.store(true, std::memory_order_release);
        const auto _current_timestamp = std::chrono::system_clock::now().time_since_epoch().count();

        const auto _runtime = _current_timestamp - timestamp;

        boost::json::object _data;
        _data.reserve(bag.size());
        for (auto &[_key, _entry]: bag) {
            _data.insert_or_assign(_key, _entry);
        }

        if (!transaction_id.is_nil()) {
            data_ = {
                {"transaction_id", to_string(transaction_id)},
                {"action", "ack"},
                {"status", "failed"},
                {"message", error},
                {"data", _data},
                {"timestamp", timestamp},
                {"runtime", _runtime}
            };
        } else {
            data_ = {
                {"transaction_id", nullptr},
                {"action", "ack"},
                {"status", "failed"},
                {"message", error},
                {"data", _data},
                {"timestamp", timestamp},
                {"runtime", _runtime}
            };
        }
    }

    void response::mark_as_processed() {
        processed_.store(true, std::memory_order_release);
    }


    void response::set_data(const boost::uuids::uuid transaction_id, const char *message, long timestamp,
                            const boost::json::object &data) {
        const auto _current_timestamp = std::chrono::system_clock::now().time_since_epoch().count();

        const auto _runtime = _current_timestamp - timestamp;

        data_ = {
            {"transaction_id", to_string(transaction_id)},
            {"action", "ack"},
            {"status", "success"},
            {"message", message},
            {"timestamp", timestamp},
            {"runtime", _runtime},
            {"data", data}
        };
    }
} // namespace aewt
