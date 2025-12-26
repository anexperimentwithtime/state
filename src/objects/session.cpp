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

#include <aewt/logger.hpp>
#include <boost/core/ignore_unused.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace aewt {
    session::session(const std::shared_ptr<state> &state, const boost::uuids::uuid id,
                     boost::asio::ip::tcp::socket &&socket)
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

        // if (socket_.is_open()) {
        // post(socket_.get_executor(), boost::beast::bind_front_handler(&session::on_send, shared_from_this(), data));
        // }
    }

    void session::run() {
        dispatch(socket_.get_executor(), boost::beast::bind_front_handler(&session::on_run, shared_from_this()));
    }

    void session::on_run() {
        socket_.async_accept(boost::beast::bind_front_handler(&session::on_accept, shared_from_this()));
    }

    void session::on_accept(const boost::beast::error_code &ec) {
        if (ec)
            return;

        do_read();
    }

    void session::do_read() {
        socket_.async_read(buffer_, boost::beast::bind_front_handler(&session::on_read, shared_from_this()));
    }

    void session::on_read(const boost::system::error_code &ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec == boost::beast::websocket::error::closed) {
            return;
        }

        if (ec) {
            return;
        }

        buffer_.consume(buffer_.size());

        do_read();
    }

    void session::on_send(std::shared_ptr<std::string const> const &data) {
        queue_.push_back(data);

        if (queue_.size() > 1)
            return;

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
