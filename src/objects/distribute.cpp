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

#include <aewt/distribute.hpp>
#include <aewt/state.hpp>
#include <aewt/session.hpp>

namespace aewt {
    std::size_t distribute_to_others(const std::shared_ptr<state> &state, const boost::json::object &data, const boost::uuids::uuid session_id) {
        auto _count = 0;
        for (auto _sessions = state->get_sessions(); const auto &_session: _sessions) {
            if (_session->get_id() != session_id) {
                _session->send(std::make_shared<boost::json::object>(data));
                _count++;
            }
        }
        return _count;
    }
} // namespace aewt
