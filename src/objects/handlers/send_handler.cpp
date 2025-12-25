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

#include <aewt/handlers/send_handler.hpp>

#include <aewt/state.hpp>
#include <aewt/session.hpp>
#include <aewt/request.hpp>

#include <aewt/validators/send_validator.hpp>

#include <aewt/utils.hpp>

namespace aewt::handlers {
    void send_handler(const request &request) {
        const auto &_state = request.state_;
        const auto &_transaction_id = request.transaction_id_;
        const auto &_session = request.session_;
        if (validators::send_validator(request)) {
            const auto &_params = get_params(request);
            const auto &_sender_id = get_param_as_id(_params, "sender_id");
            const auto &_receiver_id = get_param_as_id(_params, "receiver_id");
            const auto &_payload = get_param_as_object(_params, "payload");
            const bool _success = _state->send(_transaction_id, _session->get_id(), _sender_id, _receiver_id, _payload);
            const auto _status = get_status(_success);
            next(request, _status);
        }
    }
}
