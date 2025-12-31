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

#include <aewt/session.hpp>

#include <aewt/state.hpp>
#include <aewt/kernel.hpp>

#include <aewt/logger.hpp>
#include <aewt/response.hpp>
#include <boost/core/ignore_unused.hpp>

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>

#include <dotenv.h>

namespace aewt {
    session::session(const std::shared_ptr<state> &state,
                     boost::asio::ip::tcp::socket &&socket, const boost::uuids::uuid id)
        : state_(state), id_(id), socket_(boost::beast::tcp_stream(std::move(socket))) {
        LOG_INFO("session {} allocated", to_string(id_));
    }

    session::~session() {
        LOG_INFO("session {} released", to_string(id_));
    }

    boost::uuids::uuid session::get_id() const { return id_; }

    boost::beast::websocket::stream<boost::beast::tcp_stream> &session::get_socket() { return socket_; }

    void session::send(std::shared_ptr<std::string const> const &data) {
        boost::ignore_unused(data);

        if (socket_.is_open()) {
            LOG_INFO("session {} send data [{}]", to_string(id_), *data);
            post(socket_.get_executor(), boost::beast::bind_front_handler(&session::on_send, shared_from_this(), data));
        } else {
            LOG_INFO("session {} send socket not open", to_string(id_));
        }
    }

    void session::run(session_context context) {
        dispatch(socket_.get_executor(), boost::beast::bind_front_handler(&session::on_run, shared_from_this(), context));
    }

    unsigned short session::get_sessions_port() const {
        return sessions_port_;
    }

    void session::set_sessions_port(const unsigned short port) {
        sessions_port_ = port;
    }

    unsigned short session::get_clients_port() const {
        return clients_port_;
    }

    void session::set_clients_port(const unsigned short port) {
        clients_port_ = port;
    }

    std::string session::get_host() const {
        return host_;
    }

    void session::set_host(const std::string &host) {
        host_ = host;
    }

    void session::mark_as_registered() {
        registered_.store(true, std::memory_order_release);
    }

    bool session::get_registered() const {
        return registered_.load(std::memory_order_acquire);
    }

    void session::on_run(const session_context context) {
        switch (context) {
            case local: {
                LOG_INFO("session {} starting accept", to_string(id_));
                socket_.async_accept(boost::beast::bind_front_handler(&session::on_accept, shared_from_this(), context));
                break;
            }
            case remote: {
                LOG_INFO("session {} starting handshake", to_string(id_));
                const auto _host = fmt::format("{}:{}", dotenv::getenv("REMOTE_HOST", "127.0.0.1"), dotenv::getenv("REMOTE_PORT", "10000"));
                socket_.async_handshake(
                                _host,
                                "/",
                                boost::beast::bind_front_handler(
                                    &session::on_accept,
                                    shared_from_this(), context));
            }
        }
    }

    void session::on_accept(const session_context context, const boost::beast::error_code &ec) {
        if (ec) {
            state_->remove_session(id_);
            return;
        }

        host_ = socket_.next_layer().socket().remote_endpoint().address().to_string();

        if (context == local) {
            state_->sync(shared_from_this());
        } else {
            // Sí una sesión se registra debe declarar los puertos que provee servicios
            const boost::json::object _response = {
                {"transaction_id", to_string(boost::uuids::random_generator()())},
                {"action", "register"},
                {
                    "params", {
                            {"clients_port", state_->get_clients_port()},
                            {"sessions_port", state_->get_sessions_port()},
                        }
                },
            };
            send(std::make_shared<std::string const>(serialize(_response)));
        }

        LOG_INFO("session {} accept/handshake success", to_string(id_));
        do_read();
    }

    void session::do_read() {
        LOG_INFO("session {} starting read", to_string(id_));
        socket_.async_read(buffer_, boost::beast::bind_front_handler(&session::on_read, shared_from_this()));
    }

    void session::on_read(const boost::system::error_code &ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec == boost::beast::websocket::error::closed) {
            state_->remove_session(id_);
            return;
        }

        if (ec) {
            state_->remove_session(id_);
            return;
        }

        const auto _read_at = std::chrono::system_clock::now().time_since_epoch().count();
        auto _stream = boost::beast::buffers_to_string(buffer_.data());
        LOG_INFO("session {} read: {}", to_string(id_), _stream);

        boost::system::error_code _parse_ec;

        if (auto _data = boost::json::parse(_stream, _parse_ec); !_parse_ec && _data.is_object()) {
            if (const auto _response = kernel(state_, _data.as_object(), on_session, get_id()); !_response->is_ack()) {
                send(std::make_shared<std::string const>(serialize(_response->get_data())));
            }
        } else {
            auto _now = std::chrono::system_clock::now().time_since_epoch().count();
            const boost::json::object _response = {
                {"transaction_id", nullptr},
                {"status", "failed"},
                {"message", "unprocessable entity"},
                {
                    "data", {
                        {"body", "body must be json object"},
                    }
                },
                {"timestamp", _now},
                {"runtime", _now - _read_at},
            };
            send(std::make_shared<std::string const>(serialize(_response)));
        }

        buffer_.consume(buffer_.size());

        do_read();
    }

    void session::on_send(std::shared_ptr<std::string const> const &data) {
        queue_.push_back(data);

        if (queue_.size() > 1)
            return;

        const auto _message = *queue_.begin();
        LOG_INFO("session {} write: {}", to_string(id_), _message->data());

        socket_.async_write(boost::asio::buffer(*queue_.front()),
                            boost::beast::bind_front_handler(&session::on_write, shared_from_this()));
    }

    void session::on_write(const boost::beast::error_code &ec, std::size_t bytes_transferred) {
        if (ec)
            return;

        queue_.erase(queue_.begin());

        if (!queue_.empty())
            socket_.async_write(
                boost::asio::buffer(*queue_.front()),
                boost::beast::bind_front_handler(
                    &session::on_write,
                    shared_from_this()));
    }
} // namespace aewt
