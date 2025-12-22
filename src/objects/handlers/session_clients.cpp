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

#include <aewt/handlers/session_clients.hpp>

#include <aewt/response.hpp>
#include <aewt/state.hpp>
#include <aewt/session.hpp>

#include <aewt/validators/session_id.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace aewt::handlers {
    void session_clients(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response,
                            const std::shared_ptr<state> &state,
                            const std::shared_ptr<session> &session, const boost::json::object &data) {
        if (validators::session_id(transaction_id, response, data)) {
            const auto _timestamp = std::chrono::system_clock::now();
            auto _clients = state->get_clients_by_session(session->get_id());
            boost::json::array _clients_array;
            for (const auto &client : _clients) {
                _clients_array.push_back(to_string(client).data());
            }
            const boost::json::object _data = {
                   {"timestamp", _timestamp.time_since_epoch().count()},
                {"clients", _clients_array},
            };
            response->set_data(transaction_id, "ok", _data);
        }
    }

}
