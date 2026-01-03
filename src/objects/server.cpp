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

#include <aewt/server.hpp>

#include <aewt/state.hpp>
#include <aewt/session.hpp>

#include <aewt/logger.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <aewt/session_listener.hpp>
#include <aewt/client_listener.hpp>
#include <aewt/repl.hpp>
#include <boost/asio/strand.hpp>

namespace aewt {
    server::server(const std::shared_ptr<config> &configuration) : config_(configuration),
                                                                   state_(std::make_shared<state>(configuration)) {
    }

    void server::start() {
        auto const &_config = state_->get_config();
        auto const _address = boost::asio::ip::make_address(_config->address_);
        boost::asio::ip::tcp::resolver _resolver{make_strand(state_->get_ioc())};

        LOG_INFO("state_id=[{}] action=[running] sessions_port=[{}] clients_port=[{}]", to_string(state_->get_id()),
                 _config->sessions_port_.load(std::memory_order_acquire), _config->clients_port_.load(std::memory_order_acquire));

        if (_config->is_node_) {
            LOG_INFO("state_id=[{}] action=[waiting for remote] remote_address=[{}] remote_sessions_port=[{}]", to_string(state_->get_id()),
                     _config->remote_address_, _config->remote_sessions_port_.load(std::memory_order_acquire));

            std::this_thread::sleep_for(std::chrono::seconds(3));

            auto const _results = _resolver.resolve(_config->remote_address_,
                                                    std::to_string(_config->remote_sessions_port_.load(std::memory_order_acquire)));
            const auto _remote_session = std::make_shared<session>(
                state_, boost::asio::ip::tcp::socket{make_strand(state_->get_ioc())});
            auto &_socket = _remote_session->get_socket();
            auto &_lowest_socket = _socket.next_layer().socket().lowest_layer();

            while (!_lowest_socket.is_open()) {
                try {
                    boost::asio::connect(_lowest_socket, _results);
                } catch (std::exception &e) {
                    LOG_INFO("Connection refused ... retrying : {}", e.what());
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                }
            }
            _remote_session->set_sessions_port(_config->remote_sessions_port_.load(std::memory_order_acquire));
            _remote_session->set_clients_port(_config->remote_clients_port_.load(std::memory_order_acquire));
            _remote_session->run(remote);

            state_->add_session(_remote_session);
        }

        session_listener_ = std::make_shared<session_listener>(state_->get_ioc(),
                                                               boost::asio::ip::tcp::endpoint{
                                                                   _address, _config->sessions_port_.load(std::memory_order_acquire)
                                                               },
                                                               state_);

        session_listener_->start();

        client_listener_ = std::make_shared<client_listener>(state_->get_ioc(),
                                                             boost::asio::ip::tcp::endpoint{
                                                                 _address, _config->clients_port_.load(std::memory_order_acquire)
                                                             },
                                                             state_);

        client_listener_->start();

        if (_config->repl_enabled) {
            repl_ = std::make_unique<repl>(state_);
        }

        vector_of_threads_.reserve(config_->threads_ - 1);
        for (auto i = config_->threads_ - 1; i > 0; --i)
            vector_of_threads_.emplace_back(
                [_state = this->state_->shared_from_this()]() {
                    _state->get_ioc().run();
                });
        state_->get_ioc().run();
    }

    std::shared_ptr<config> server::get_config() {
        return config_;
    }

    std::shared_ptr<state> server::get_state() {
        return state_;
    }

    void server::stop() const {
        state_->get_ioc().stop();
    }
} // namespace aewt
