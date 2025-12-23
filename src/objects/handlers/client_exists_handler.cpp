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

#include <aewt/handlers/client_exists_handler.hpp>

#include <aewt/validators/clients_validator.hpp>

#include <aewt/response.hpp>
#include <aewt/state.hpp>
#include <aewt/session.hpp>

#include <boost/core/ignore_unused.hpp>

#include <aewt/utils.hpp>

namespace aewt::handlers {
    void client_exists_handler(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response,
                               const std::shared_ptr<state> &state, const std::shared_ptr<session> &session,
                               const boost::json::object &data, const long timestamp) {
        boost::ignore_unused(session);

        if (validators::clients_validator(transaction_id, response, data, timestamp)) {
            auto _params = data.at("params").as_object();
            const auto _client_id = GET_PARAM_AS_ID(_params, "client_id");

            const auto _status = state->get_client_exists(_client_id) ? "yes" : "no";

            response->set_data(transaction_id, _status, timestamp);
        }
    }
}
