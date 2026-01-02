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
#include <aewt/kernel_context.hpp>

#include <aewt/validators/send_validator.hpp>

#include <aewt/utils.hpp>

#include <boost/uuid/uuid_io.hpp>
#include <boost/json/serialize.hpp>
#include <aewt/logger.hpp>

namespace aewt::handlers {
    void send_handler(const request &request) {
        auto &_state = request.state_;

        if (validators::send_validator(request)) {
            const auto &_params = get_params(request);
            const auto _to_client_id = get_param_as_id(_params, "to_client_id");
            const auto &_payload = get_param_as_object(_params, "payload");
            switch (request.context_) {
                case on_client: {
                    if (const auto _client = _state->get_client(_to_client_id); _client.has_value()) {
                        if (const auto &_scoped_client = _client.value();
                            _scoped_client->get_session_id() == _state->get_id()) {
                            const boost::json::object _data = {
                                {"transaction_id", to_string(boost::uuids::random_generator()())},
                                {"action", "send"},
                                {
                                    "params", {
                                        {"from_client_id", to_string(request.entity_id_)},
                                        {"to_client_id", to_string(_scoped_client->get_id())},
                                        {"payload", _payload},
                                    }
                                }
                            };
                            _scoped_client->send(std::make_shared<std::string const>(serialize(_data)));

                            LOG_INFO("state_id=[{}] action=[send] context=[{}] from_client_id=[{}] to_client_id=[{}] status=[ok] size=[{}]", to_string(request.state_->get_id()), kernel_context_to_string(request.context_), to_string(request.entity_id_), to_string(_scoped_client->get_id()), _payload.size());

                            next(request, "ok");
                        } else {
                            _state->send_to_session(_scoped_client->get_session_id(), request.entity_id_,
                                                    _scoped_client->get_id(), _payload);

                            LOG_INFO("state_id=[{}] action=[send] context=[{}] from_client_id=[{}] to_client_id=[{}] status=[ok] size=[{}]", to_string(request.state_->get_id()), kernel_context_to_string(request.context_), to_string(request.entity_id_), to_string(_scoped_client->get_id()), _payload.size());

                            next(request, "ok");
                        }
                    } else {
                        next(request, "no effect");
                    }
                    break;
                }
                case on_session: {
                    const auto _from_client_id = get_param_as_id(_params, "from_client_id");
                    if (const auto _client = _state->get_client(_to_client_id); _client.has_value()) {
                        if (const auto &_scoped_client = _client.value();
                            _scoped_client->get_session_id() == _state->get_id()) {
                            const boost::json::object _data = {
                                {"transaction_id", to_string(boost::uuids::random_generator()())},
                                {"action", "send"},
                                {
                                    "params", {
                                        {"from_client_id", to_string(_from_client_id)},
                                        {"to_client_id", to_string(_scoped_client->get_id())},
                                        {"payload", _payload},
                                    }
                                }
                            };
                            _scoped_client->send(std::make_shared<std::string const>(serialize(_data)));

                            LOG_INFO("state_id=[{}] action=[send] context=[{}] from_client_id=[{}] to_client_id=[{}] status=[ok] size=[{}]", to_string(request.state_->get_id()), kernel_context_to_string(request.context_), to_string(_from_client_id), to_string(_scoped_client->get_id()), _payload.size());
                            next(request, "ok");
                        } else {
                            next(request, "no effect");
                        }
                    } else {
                        next(request, "no effect");
                    }
                    break;
                }
            }
        }
    }
}
