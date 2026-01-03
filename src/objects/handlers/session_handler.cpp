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
#include <boost/asio/strand.hpp>

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
                    for (const auto &_session: _state->get_sessions()) {
                        if (_session->get_host() == _host && _session->get_sessions_port() == _sessions_port && _session
                            ->get_clients_port() == _clients_port) {
                            _found = true;
                        }
                    }

                    if (!_found) {
                        boost::asio::ip::tcp::resolver _resolver{make_strand(_state->get_ioc())};
                        auto const _results = _resolver.resolve(_host, std::to_string(_sessions_port));
                        const auto _remote_session = std::make_shared<session>(
                            _state, boost::asio::ip::tcp::socket{make_strand(_state->get_ioc())});
                        auto &_socket = _remote_session->get_socket();
                        auto &_lowest_socket = _socket.next_layer().socket().lowest_layer();
                        while (!_lowest_socket.is_open()) {
                            try {
                                boost::asio::connect(_lowest_socket, _results);
                            } catch (std::exception &e) {
                                LOG_INFO("Connection refused ... retrying : {}", e.what());
                                boost::system::error_code _ec;
                                _lowest_socket.close(_ec);
                                std::this_thread::sleep_for(std::chrono::seconds(3));
                            }
                        }
                        _remote_session->set_clients_port(_clients_port);
                        _remote_session->set_sessions_port(_sessions_port);
                        _remote_session->run(remote);
                        _state->add_session(_remote_session);
                        LOG_INFO(
                            "state_id=[{}] action=[session] context=[{}] session_id=[{}] host=[{}] sessions_port=[{}] clients_port=[{}] status=[ok]",
                            to_string(request.state_->get_id()), kernel_context_to_string(request.context_),
                            to_string(_remote_session->get_id()), _host, _sessions_port, _clients_port);

                        next(request, "ok");
                    } else {
                        LOG_INFO("state_id=[{}] action=[session] context=[{}] status=[no effect]",
                                 to_string(request.state_->get_id()), kernel_context_to_string(request.context_));

                        next(request, "no effect");
                    }
                }
                break;
            }
        }
    }
}
