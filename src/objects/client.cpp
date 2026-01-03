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

#include <aewt/client.hpp>

#include <aewt/logger.hpp>
#include <aewt/state.hpp>
#include <aewt/kernel.hpp>
#include <aewt/response.hpp>

#include <boost/core/ignore_unused.hpp>

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>

namespace aewt {
    client::client(const boost::uuids::uuid session_id,
                   const std::shared_ptr<state> &state, const boost::uuids::uuid id) : state_(state),
        id_(id),
        session_id_(session_id),
        is_local_(state->get_id() == session_id) {
        LOG_INFO("state_id=[{}] action=[client_allocated] session_id=[{}] client_id=[{}]", to_string(state_->get_id()),
                 to_string(session_id), to_string(id_));
    }

    client::~client() {
        LOG_INFO("state_id=[{}] action=[client_released] session_id=[{}] client_id=[{}]", to_string(state_->get_id()),
                 to_string(get_session_id()), to_string(id_));
    }

    boost::uuids::uuid client::get_id() const { return id_; }

    boost::uuids::uuid client::get_session_id() const {
        return session_id_;
    }

    bool client::get_is_local() const {
        return is_local_;
    }

    void client::run() {
        auto _run_at = std::chrono::system_clock::now().time_since_epoch().count();
        if (socket_.has_value()) {
            auto &_socket = socket_.value();
            _socket.async_accept(boost::beast::bind_front_handler(&client::on_accept, shared_from_this(), _run_at));
        }
    }

    void client::send(std::shared_ptr<std::string const> const &data) {
        boost::ignore_unused(data);

        if (socket_.has_value()) {
            if (auto &_socket = socket_.value(); _socket.is_open()) {
                post(_socket.get_executor(),
                     boost::beast::bind_front_handler(&client::on_send, shared_from_this(), data));
            }
        }
    }

    void client::set_socket(boost::asio::ip::tcp::socket &&socket) {
        socket_.emplace(std::move(socket));
    }

    void client::on_accept(long run_at, const boost::beast::error_code &ec) {
        if (ec) {
            state_->remove_client(id_);
            const auto _ = state_->leave_to_sessions(get_id());
            boost::ignore_unused(_);
            return;
        }

        auto _now = std::chrono::system_clock::now().time_since_epoch().count();
        const boost::json::object _welcome = {
            {"transaction_id", to_string(boost::uuids::random_generator()())},
            {"action", "welcome"},
            {"status", "success"},
            {"message", "accepted"},
            {"timestamp", _now},
            {"runtime", _now - run_at},
            {"data", {{"client_id", to_string(get_id())}}},
        };
        send(std::make_shared<std::string const>(serialize(_welcome)));

        do_read();
    }

    void client::do_read() {
        auto &_socket = socket_.value();
        _socket.async_read(buffer_, boost::beast::bind_front_handler(&client::on_read, shared_from_this()));
    }

    void client::on_read(const boost::system::error_code &ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec == boost::beast::websocket::error::closed) {
            state_->remove_client(id_);
            const auto _ = state_->leave_to_sessions(get_id());
            boost::ignore_unused(_);
            return;
        }

        if (ec) {
            state_->remove_client(id_);
            const auto _ = state_->leave_to_sessions(get_id());
            boost::ignore_unused(_);
            return;
        }

        const auto _read_at = std::chrono::system_clock::now().time_since_epoch().count();
        auto _stream = boost::beast::buffers_to_string(buffer_.data());

        boost::system::error_code _parse_ec;

        if (auto _data = boost::json::parse(_stream, _parse_ec); !_parse_ec && _data.is_object()) {
            const auto _response = kernel(state_, _data.as_object(), on_client, get_id());
            send(std::make_shared<std::string const>(serialize(_response->get_data())));
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

    void client::on_send(std::shared_ptr<std::string const> const &data) {
        queue_.push_back(data);

        if (queue_.size() > 1)
            return;

        const auto _message = *queue_.begin();

        if (socket_.has_value()) {
            if (auto &_socket = socket_.value(); _socket.is_open()) {
                _socket.async_write(boost::asio::buffer(*queue_.front()),
                                    boost::beast::bind_front_handler(&client::on_write, shared_from_this()));
            }
        }
    }

    void client::on_write(const boost::beast::error_code &ec, std::size_t bytes_transferred) {
        if (ec)
            return;

        queue_.erase(queue_.begin());

        if (!queue_.empty() && socket_.has_value()) {
            if (auto &_socket = socket_.value(); _socket.is_open()) {
                _socket.async_write(
                    boost::asio::buffer(*queue_.front()),
                    boost::beast::bind_front_handler(
                        &client::on_write,
                        shared_from_this()));
            }
        }
    }


    std::optional<boost::beast::websocket::stream<boost::beast::tcp_stream> > &client::get_socket() { return socket_; }
} // namespace aewt
