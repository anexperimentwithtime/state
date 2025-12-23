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

#include <aewt/handlers/client_join_handler.hpp>

#include <aewt/validators/clients_validator.hpp>

#include <aewt/response.hpp>
#include <aewt/state.hpp>
#include <aewt/session.hpp>
#include <aewt/request.hpp>

#include <aewt/utils.hpp>
#include <aewt/distribute.hpp>

namespace aewt::handlers {
    /**
     * @details
     *
     *  Aceptar a un cliente requiere que todas las sesiones sean informados.
     *
     */
    void client_join_handler(const request &request) {
        if (validators::clients_validator(request)) {
            auto _params = request.data.at("params").as_object();
            const auto _client_id = GET_PARAM_AS_ID(_params, "client_id");
            const auto _session_id = GET_PARAM_AS_ID(_params, "session_id");

            const auto _is_local = request.session->get_id() == _session_id;
            const auto _inserted = request.state->add_client(_client_id, _session_id, _is_local);

            std::size_t _count = _is_local ? distribute_to_others(request.state, request.data) : 0;

            const auto _status = _inserted ? "ok" : "no effect";

            request.response->set_data(
                request.transaction_id,
                _status,
                request.timestamp,
                {
                    {"count", _count}
                });
        }
    }
}
