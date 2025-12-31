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

#include <aewt/logger.hpp>
#include <aewt/handlers/session_handler.hpp>

#include <aewt/state.hpp>
#include <aewt/request.hpp>
#include <aewt/session.hpp>

#include <aewt/validators/session_validator.hpp>

#include <aewt/utils.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace aewt::handlers {
    void session_handler(const request &request) {
        auto &_state = request.state_;

        switch (request.context_) {
            case on_client: {
                next(request, "no effect");
                break;
            }
            case on_session: {
                if (validators::session_validator(request)) {
                    const auto &_params = get_params(request);
                    const auto _host = get_param_as_string(_params, "host");
                    const auto _sessions_port = get_param_as_number(_params, "sessions_port");
                    const auto _clients_port = get_param_as_number(_params, "clients_port");


                    bool _found = false;
                    for (const auto _session : _state->get_sessions()) {
                        if (auto &_socket = _session->get_socket().next_layer().socket(); _socket.remote_endpoint().address().to_string() == _host && _socket.remote_endpoint().port() == _sessions_port) {
                            _found = true;
                        }
                    }

                    if (!_found) {
                        boost::asio::ip::tcp::resolver _resolver{_state->get_ioc()};
                        auto const _results = _resolver.resolve(_host, std::to_string(_sessions_port));
                        const auto _remote_session = std::make_shared<session>(_state, boost::asio::ip::tcp::socket { _state->get_ioc() });
                        auto & _socket = _remote_session->get_socket();
                        auto & _lowest_socket = _socket.next_layer().socket().lowest_layer();
                        boost::asio::connect(_lowest_socket, _results);
                        _remote_session->set_clients_port(_clients_port);
                        _remote_session->set_sessions_port(_sessions_port);
                        _remote_session->run(remote);
                        _state->add_session(_remote_session);
                        next(request, "ok");
                    } else {
                        LOG_INFO("session already exists host={} port={}", _host, _sessions_port);
                        next(request, "no effect");
                    }
                }
                break;
            }
        }
    }
}
