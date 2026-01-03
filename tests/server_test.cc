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

#include <gtest/gtest.h>

#include "server_base.hpp"
#include <aewt/logger.hpp>
#include <boost/json/serialize.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/asio/strand.hpp>

TEST_F(server_test, assert_local_server_is_registered) {
    ASSERT_TRUE(_local_server->get_config()->registered_);
}

TEST_F(server_test, assert_local_server_accept_clients) {
    boost::asio::io_context _ioc;
    boost::asio::ip::tcp::resolver _resolver{_ioc};
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _client{_ioc};

    auto const _results = _resolver.resolve("127.0.0.1", std::to_string(_local_server->get_config()->clients_port_.load(std::memory_order_acquire)));
    boost::asio::connect(_client.next_layer(), _results);

    const auto _host = fmt::format("127.0.0.1:{}", std::to_string(_local_server->get_config()->clients_port_.load(std::memory_order_acquire)));
    _client.handshake(_host, "/");

    boost::beast::flat_buffer _accepted_buffer;
    _client.read(_accepted_buffer);
    std::cout << boost::beast::make_printable(_accepted_buffer.data()) << std::endl;

    ASSERT_TRUE(_local_server->get_state()->get_clients().size() == 1);
    _client.close(boost::beast::websocket::close_code::normal);

    // Wait for processing.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    ASSERT_TRUE(_local_server->get_state()->get_clients().size() == 0);
}

TEST_F(server_test, assert_local_server_can_handle_subscribe) {
    boost::asio::io_context _ioc;
    boost::asio::ip::tcp::resolver _resolver{_ioc};
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _client{_ioc};

    auto const _results = _resolver.resolve("127.0.0.1", std::to_string(_local_server->get_config()->clients_port_.load(std::memory_order_acquire)));
    boost::asio::connect(_client.next_layer(), _results);

    const auto _host = fmt::format("127.0.0.1:{}", std::to_string(_local_server->get_config()->clients_port_.load(std::memory_order_acquire)));
    _client.handshake(_host, "/");

    boost::beast::flat_buffer _accepted_buffer;
    _client.read(_accepted_buffer);
    std::cout << boost::beast::make_printable(_accepted_buffer.data()) << std::endl;

    ASSERT_TRUE(_local_server->get_state()->get_clients().size() == 1);

    _client.write(boost::asio::buffer(std::string(serialize(boost::json::object{
        {"transaction_id", to_string(boost::uuids::random_generator()())},
        {"action", "subscribe"},
        {"params", {{"channel", "welcome"}}},
    }))));

    boost::beast::flat_buffer _buffer;

    _client.read(_buffer);

    std::cout << boost::beast::make_printable(_buffer.data()) << std::endl;

    _client.close(boost::beast::websocket::close_code::normal);
}

TEST_F(server_test, assert_local_server_can_handle_publish) {
    boost::asio::io_context _ioc;
    boost::asio::ip::tcp::resolver _resolver{_ioc};
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _local_client{make_strand(_ioc)};
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _other_local_client{make_strand(_ioc)};
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _remote_client{make_strand(_ioc)};
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> _other_remote_client{make_strand(_ioc)};

    {
        auto const _results = _resolver.resolve("127.0.0.1", std::to_string(_local_server->get_config()->clients_port_.load(std::memory_order_acquire)));
        boost::asio::connect(_local_client.next_layer(), _results);
        boost::asio::connect(_other_local_client.next_layer(), _results);
    }

    {
        auto const _results = _resolver.resolve("127.0.0.1", std::to_string(_remote_server->get_config()->clients_port_.load(std::memory_order_acquire)));
        boost::asio::connect(_remote_client.next_layer(), _results);
        boost::asio::connect(_other_remote_client.next_layer(), _results);
    }

    const auto _host = fmt::format("127.0.0.1:{}", std::to_string(_local_server->get_config()->clients_port_.load(std::memory_order_acquire)));
    _local_client.handshake(_host, "/");
    _other_local_client.handshake(_host, "/");

    _remote_client.handshake(_host, "/");
    _other_remote_client.handshake(_host, "/");

    {
        boost::beast::flat_buffer _buffer;
        _local_client.read(_buffer);
        std::cout << "Local client should receive accepted ..." << std::endl;
        std::cout << boost::beast::make_printable(_buffer.data()) << std::endl << std::endl;
    }

    {
        boost::beast::flat_buffer _buffer;
        _other_local_client.read(_buffer);
        std::cout << "Other local client should receive accepted ..." << std::endl;
        std::cout << boost::beast::make_printable(_buffer.data()) << std::endl << std::endl;
    }

    {
        boost::beast::flat_buffer _buffer;
        _remote_client.read(_buffer);
        std::cout << "Remote client should receive accepted ..." << std::endl;
        std::cout << boost::beast::make_printable(_buffer.data()) << std::endl << std::endl;
    }

    {
        boost::beast::flat_buffer _buffer;
        _other_remote_client.read(_buffer);
        std::cout << "Other remote client should receive accepted ..." << std::endl;
        std::cout << boost::beast::make_printable(_buffer.data()) << std::endl << std::endl;
        _buffer.clear();
    }

    _local_client.write(boost::asio::buffer(std::string(serialize(boost::json::object{
        {"transaction_id", to_string(boost::uuids::random_generator()())},
        {"action", "subscribe"},
        {"params", {{"channel", "welcome"}}},
    }))));

    {
        boost::beast::flat_buffer _buffer;
        _local_client.read(_buffer);
        std::cout << "Local client should receive subscribe ack ..." << std::endl;
        std::cout << boost::beast::make_printable(_buffer.data()) << std::endl << std::endl;
        _buffer.clear();
    }

    _remote_client.write(boost::asio::buffer(std::string(serialize(boost::json::object{
    {"transaction_id", to_string(boost::uuids::random_generator()())},
    {"action", "subscribe"},
    {"params", {{"channel", "welcome"}}},
    }))));

    {
        boost::beast::flat_buffer _buffer;
        _remote_client.read(_buffer);
        std::cout << "Remote client should receive subscribe ack ..." << std::endl;
        std::cout << boost::beast::make_printable(_buffer.data()) << std::endl << std::endl;
        _buffer.clear();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    _other_local_client.write(boost::asio::buffer(std::string(serialize(boost::json::object{
        {"transaction_id", to_string(boost::uuids::random_generator()())},
        {"action", "publish"},
        {"params", {{"channel", "welcome"}, {"payload", {{"message", "EHLO"}}}}},
    }))));

    {
        boost::beast::flat_buffer _buffer;
        _other_local_client.read(_buffer);
        std::cout << "Other local client should receive publish ack ..." << std::endl;
        std::cout << boost::beast::make_printable(_buffer.data()) << std::endl << std::endl;
        _buffer.clear();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        boost::beast::flat_buffer _buffer;
        _local_client.read(_buffer);
        std::cout << "Local client should receive publish message ..." << std::endl;
        std::cout << boost::beast::make_printable(_buffer.data()) << std::endl << std::endl;
        _buffer.clear();
    }


    {
        boost::beast::flat_buffer _buffer;
        _remote_client.read(_buffer);
        std::cout << "Remote client should receive publish message ..." << std::endl;
        std::cout << boost::beast::make_printable(_buffer.data()) << std::endl << std::endl;
    }

    _local_client.close(boost::beast::websocket::close_code::normal);
    _other_local_client.close(boost::beast::websocket::close_code::normal);
    _remote_client.close(boost::beast::websocket::close_code::normal);
    _other_remote_client.close(boost::beast::websocket::close_code::normal);
}
