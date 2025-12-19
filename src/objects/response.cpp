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

namespace aewt {
    bool response::get_failed() const {
        return failed_.load(std::memory_order_acquire);
    }

    bool response::get_processed() const {
        return processed_.load(std::memory_order_acquire);
    }

    boost::json::object response::get_data() const { return data_; }

    void response::mark_as_processed() {
        processed_.store(true, std::memory_order_release);
    }

    void response::set_data(const char *message, const boost::json::object &data) {
        data_ = {{"status", "success"}, {"message", message}, {"data", data}};
    }

    void response::mark_as_failed(const char *error,
                                  const std::map<std::string, std::string> &bag) {
        failed_.store(true, std::memory_order_release);

        boost::json::object _data;
        _data.reserve(bag.size());
        for (auto &[_key, _entry]: bag) {
            _data.insert_or_assign(_key, _entry);
        }

        data_ = {{"status", "failed"}, {"message", error}, {"data", _data}};
    }
} // namespace aewt
