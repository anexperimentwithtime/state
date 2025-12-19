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

#include <aewt/kernel.hpp>
#include <aewt/response.hpp>
#include <aewt/session.hpp>
#include <aewt/state.hpp>
#include <boost/json/serialize.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

TEST(kernel_test, can_handle_non_valid_data) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));

    const boost::json::object _data = {{"", ""}};

    const auto _response = kernel(_state, _session, _data);

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());
    ASSERT_TRUE(_response->get_data().contains("status"));
    ASSERT_TRUE(_response->get_data().at("status").is_string());
    ASSERT_EQ(_response->get_data().at("status").as_string(), "failed");
    ASSERT_TRUE(_response->get_data().contains("message"));
    ASSERT_TRUE(_response->get_data().at("message").is_string());
    ASSERT_EQ(_response->get_data().at("message").as_string(), "unprocessable entity");
    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("action"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("action").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("action").as_string(),
              "action attribute must be present");
}

TEST(kernel_test, can_handle_non_actionable_data) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));

    const boost::json::object _data = {{"action", 7}};

    const auto _response = kernel(_state, _session, _data);

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());
    ASSERT_TRUE(_response->get_data().contains("status"));
    ASSERT_TRUE(_response->get_data().at("status").is_string());
    ASSERT_EQ(_response->get_data().at("status").as_string(), "failed");
    ASSERT_TRUE(_response->get_data().contains("message"));
    ASSERT_TRUE(_response->get_data().at("message").is_string());
    ASSERT_EQ(_response->get_data().at("message").as_string(), "unprocessable entity");
    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("action"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("action").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("action").as_string(), "action attribute must be string");
}

TEST(kernel_test, can_handle_non_implemented_action) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));

    const boost::json::object _data = {{"action", "invalid"}};

    const auto _response = kernel(_state, _session, _data);

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());
    ASSERT_TRUE(_response->get_data().contains("status"));
    ASSERT_TRUE(_response->get_data().at("status").is_string());
    ASSERT_EQ(_response->get_data().at("status").as_string(), "failed");
    ASSERT_TRUE(_response->get_data().contains("message"));
    ASSERT_TRUE(_response->get_data().at("message").is_string());
    ASSERT_EQ(_response->get_data().at("message").as_string(), "unprocessable entity");
    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("action"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("action").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("action").as_string(),
              "action attribute isn't implemented");
}

TEST(kernel_test, can_handle_pong) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));

    const boost::json::object _data = {{"action", "ping"}};

    const auto _response = kernel(_state, _session, _data);

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());
    ASSERT_TRUE(_response->get_data().contains("status"));
    ASSERT_TRUE(_response->get_data().at("status").is_string());
    ASSERT_EQ(_response->get_data().at("status").as_string(), "success");
    ASSERT_TRUE(_response->get_data().contains("message"));
    ASSERT_TRUE(_response->get_data().at("message").is_string());
    ASSERT_EQ(_response->get_data().at("message").as_string(), "pong");
    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("timestamp"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("timestamp").is_number());
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("timestamp").as_int64() > 0);
    ASSERT_TRUE(
        _response->get_data().at("data").as_object().at("timestamp").as_int64() < std::chrono::system_clock::now().
        time_since_epoch().count());
}

TEST(kernel_test, can_handle_whoami) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));

    const boost::json::object _data = {{"action", "whoami"}};

    const auto _response = kernel(_state, _session, _data);

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());
    ASSERT_TRUE(_response->get_data().contains("status"));
    ASSERT_TRUE(_response->get_data().at("status").is_string());
    ASSERT_EQ(_response->get_data().at("status").as_string(), "success");
    ASSERT_TRUE(_response->get_data().contains("message"));
    ASSERT_TRUE(_response->get_data().at("message").is_string());
    ASSERT_EQ(_response->get_data().at("message").as_string(), "im");
    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("id"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("id").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("id").as_string(), to_string(_session->get_id()));
}
