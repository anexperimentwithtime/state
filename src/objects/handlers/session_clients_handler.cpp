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

#include <aewt/handlers/session_clients_handler.hpp>

#include <aewt/response.hpp>
#include <aewt/state.hpp>
#include <aewt/session.hpp>
#include <aewt/request.hpp>

#include <aewt/validators/session_id_validator.hpp>

#include <aewt/utils.hpp>

namespace aewt::handlers {
    void session_clients_handler(const request &request) {
        if (validators::session_id_validator(request)) {
            const auto _params = request.data_.at("params").as_object();
            const auto _session_id = GET_PARAM_AS_ID(_params, "session_id");

            auto _clients = request.state_->get_clients_by_session(_session_id);

            boost::json::array _clients_array;
            for (const auto &client: _clients) {
                _clients_array.push_back(to_string(client).data());
            }

            const boost::json::object _data = {
                {"clients", _clients_array},
            };

            request.response_->set_data(request.transaction_id_, "ok", request.timestamp_, _data);
        }
    }
}
