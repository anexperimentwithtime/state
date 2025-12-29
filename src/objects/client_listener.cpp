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

#include <aewt/client_listener.hpp>

#include <boost/asio/strand.hpp>

#include <aewt/logger.hpp>
#include <aewt/client.hpp>
#include <aewt/state.hpp>
#include <boost/uuid/random_generator.hpp>

namespace aewt {
    client_listener::client_listener(boost::asio::io_context &ioc, const boost::asio::ip::tcp::endpoint &endpoint,
                                     const std::shared_ptr<state> &state) : ioc_(ioc), acceptor_(ioc), state_(state) {
        boost::beast::error_code ec;

        acceptor_.open(endpoint.protocol(), ec);
        if (ec) {
            LOG_INFO("listener failed on open: {}", ec.what());
            return;
        }

        acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec) {
            LOG_INFO("listener failed on set option reuse address: {}", ec.what());
            return;
        }

        acceptor_.bind(endpoint, ec);
        if (ec) {
            LOG_INFO("listener failed on bind: {}", ec.what());
            return;
        }

        acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec) {
            LOG_INFO("listener failed on listen: {}", ec.what());
            return;
        }
    }

    void client_listener::on_accept(const boost::beast::error_code &ec, boost::asio::ip::tcp::socket socket) {
        if (ec) {
            LOG_INFO("listener failed on accept: {}", ec.what());
        } else {
            const auto _client = std::make_shared<client>(boost::uuids::random_generator()(), state_->get_id(), state_);
            _client->set_socket(std::move(socket));
            state_->add_client(_client);
            _client->run();
        }

        do_accept();
    }

    void client_listener::do_accept() {
        acceptor_.async_accept(
            make_strand(ioc_),
            boost::beast::bind_front_handler(
                &client_listener::on_accept,
                shared_from_this()));
    }

    void client_listener::start() {
        do_accept();
    }
} // namespace aewt
