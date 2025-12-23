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

#include <aewt/handlers/whoami_handler.hpp>

#include <aewt/response.hpp>
#include <aewt/session.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace aewt::handlers {
    void whoami_handler(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response,
                        const std::shared_ptr<session> &session, const long timestamp) {
        const auto &_socket = session->get_socket();
        boost::json::object _data = {
            {"id", to_string(session->get_id())},
            {"is_open", _socket.is_open()},
        };
        if (_socket.is_open()) {
            const auto _remote_endpoint = _socket.remote_endpoint();
            _data["ip"] = _remote_endpoint.address().to_string();
            _data["port"] = _remote_endpoint.port();
        } else {
            _data["ip"] = nullptr;
            _data["port"] = nullptr;
        }

        response->set_data(transaction_id, "ok", timestamp, _data);
    }
}
