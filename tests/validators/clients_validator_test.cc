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
#include <aewt/logger.hpp>
#include <boost/json/serialize.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

TEST(validators_clients_validator_test, can_handle_empty_params_client_id_on_clients) {
    const auto _state = std::make_shared<aewt::state>();
    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));
    for (const auto _action: {"client_join", "client_leave"}) {
        auto _transaction_id = to_string(_state->get_generator()());
        const boost::json::object _data = {
            {"action", _action}, {"transaction_id", _transaction_id},
            {"params", {{"session_id", to_string(_session->get_id())}}}
        };

        const auto _response = kernel(_state, _session, _data);

        LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
                 serialize(_response->get_data()));

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
        ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
        ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
        ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
                  "params client_id attribute must be present");

        ASSERT_TRUE(_response->get_data().contains("runtime"));
        ASSERT_TRUE(_response->get_data().at("runtime").is_number());
        ASSERT_TRUE(_response->get_data().at("runtime").as_int64() > 0);

        ASSERT_TRUE(_response->get_data().contains("timestamp"));
        ASSERT_TRUE(_response->get_data().at("timestamp").is_number());
        ASSERT_TRUE(_response->get_data().at("timestamp").as_int64() > 0);
        ASSERT_TRUE(
            _response->get_data().at("timestamp").as_int64() < std::chrono::system_clock::now().
            time_since_epoch().count());

        ASSERT_TRUE(_response->get_data().contains("transaction_id"));
        ASSERT_TRUE(_response->get_data().at("transaction_id").is_string());
        ASSERT_EQ(_response->get_data().at("transaction_id").as_string(), _transaction_id);
    }
}

TEST(validators_clients_validator_test, can_handle_wrong_params_client_id_primivite_on_clients) {
    const auto _state = std::make_shared<aewt::state>();
    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));
    for (const auto _action: {"client_join", "client_leave"}) {
        auto _transaction_id = to_string(_state->get_generator()());
        const boost::json::object _data = {
            {"action", _action}, {"transaction_id", _transaction_id},
            {"params", {{"session_id", to_string(_session->get_id())}, {"client_id", 7}}}
        };

        const auto _response = kernel(_state, _session, _data);

        LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
                 serialize(_response->get_data()));

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
        ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
        ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
        ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
                  "params client_id attribute must be string");

        ASSERT_TRUE(_response->get_data().contains("runtime"));
        ASSERT_TRUE(_response->get_data().at("runtime").is_number());
        ASSERT_TRUE(_response->get_data().at("runtime").as_int64() > 0);

        ASSERT_TRUE(_response->get_data().contains("timestamp"));
        ASSERT_TRUE(_response->get_data().at("timestamp").is_number());
        ASSERT_TRUE(_response->get_data().at("timestamp").as_int64() > 0);
        ASSERT_TRUE(
            _response->get_data().at("timestamp").as_int64() < std::chrono::system_clock::now().
            time_since_epoch().count());

        ASSERT_TRUE(_response->get_data().contains("transaction_id"));
        ASSERT_TRUE(_response->get_data().at("transaction_id").is_string());
        ASSERT_EQ(_response->get_data().at("transaction_id").as_string(), _transaction_id);
    }
}

TEST(validators_clients_validator_test, can_handle_wrong_params_client_id_type_on_clients) {
    const auto _state = std::make_shared<aewt::state>();
    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));
    for (const auto _action: {"client_join", "client_leave"}) {
        auto _transaction_id = to_string(_state->get_generator()());
        const boost::json::object _data = {
            {"action", _action}, {"transaction_id", _transaction_id},
            {"params", {{"session_id", to_string(_session->get_id())}, {"client_id", "7"}}}
        };

        const auto _response = kernel(_state, _session, _data);

        LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
                 serialize(_response->get_data()));

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
        ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
        ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
        ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
                  "params client_id attribute must be uuid");

        ASSERT_TRUE(_response->get_data().contains("runtime"));
        ASSERT_TRUE(_response->get_data().at("runtime").is_number());
        ASSERT_TRUE(_response->get_data().at("runtime").as_int64() > 0);

        ASSERT_TRUE(_response->get_data().contains("timestamp"));
        ASSERT_TRUE(_response->get_data().at("timestamp").is_number());
        ASSERT_TRUE(_response->get_data().at("timestamp").as_int64() > 0);
        ASSERT_TRUE(
            _response->get_data().at("timestamp").as_int64() < std::chrono::system_clock::now().
            time_since_epoch().count());

        ASSERT_TRUE(_response->get_data().contains("transaction_id"));
        ASSERT_TRUE(_response->get_data().at("transaction_id").is_string());
        ASSERT_EQ(_response->get_data().at("transaction_id").as_string(), _transaction_id);
    }
}

TEST(validators_clients_validator_test, can_handle_empty_params_session_id_on_clients) {
    const auto _state = std::make_shared<aewt::state>();
    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));
    for (const auto _action: {"client_join", "client_leave"}) {
        auto _transaction_id = to_string(_state->get_generator()());
        auto _client_id = to_string(_state->get_generator()());
        const boost::json::object _data = {
            {"action", _action}, {"transaction_id", _transaction_id}, {"params", {{"client_id", _client_id}}}
        };

        const auto _response = kernel(_state, _session, _data);

        LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
                 serialize(_response->get_data()));

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
        ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
        ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
        ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
                  "params session_id attribute must be present");

        ASSERT_TRUE(_response->get_data().contains("runtime"));
        ASSERT_TRUE(_response->get_data().at("runtime").is_number());
        ASSERT_TRUE(_response->get_data().at("runtime").as_int64() > 0);

        ASSERT_TRUE(_response->get_data().contains("timestamp"));
        ASSERT_TRUE(_response->get_data().at("timestamp").is_number());
        ASSERT_TRUE(_response->get_data().at("timestamp").as_int64() > 0);
        ASSERT_TRUE(
            _response->get_data().at("timestamp").as_int64() < std::chrono::system_clock::now().
            time_since_epoch().count());

        ASSERT_TRUE(_response->get_data().contains("transaction_id"));
        ASSERT_TRUE(_response->get_data().at("transaction_id").is_string());
        ASSERT_EQ(_response->get_data().at("transaction_id").as_string(), _transaction_id);
    }
}

TEST(validators_clients_validator_test, can_handle_wrong_params_session_id_primivite_on_clients) {
    const auto _state = std::make_shared<aewt::state>();
    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));
    for (const auto _action: {"client_join", "client_leave"}) {
        auto _transaction_id = to_string(_state->get_generator()());
        auto _client_id = to_string(_state->get_generator()());
        const boost::json::object _data = {
            {"action", _action}, {"transaction_id", _transaction_id},
            {"params", {{"session_id", 7}, {"client_id", _client_id}}}
        };
        const auto _response = kernel(_state, _session, _data);

        LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
                 serialize(_response->get_data()));

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
        ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
        ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
        ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
                  "params session_id attribute must be string");

        ASSERT_TRUE(_response->get_data().contains("runtime"));
        ASSERT_TRUE(_response->get_data().at("runtime").is_number());
        ASSERT_TRUE(_response->get_data().at("runtime").as_int64() > 0);

        ASSERT_TRUE(_response->get_data().contains("timestamp"));
        ASSERT_TRUE(_response->get_data().at("timestamp").is_number());
        ASSERT_TRUE(_response->get_data().at("timestamp").as_int64() > 0);
        ASSERT_TRUE(
            _response->get_data().at("timestamp").as_int64() < std::chrono::system_clock::now().
            time_since_epoch().count());

        ASSERT_TRUE(_response->get_data().contains("transaction_id"));
        ASSERT_TRUE(_response->get_data().at("transaction_id").is_string());
        ASSERT_EQ(_response->get_data().at("transaction_id").as_string(), _transaction_id);
    }
}

TEST(validators_clients_validator_test, can_handle_wrong_params_session_id_type_on_clients) {
    const auto _state = std::make_shared<aewt::state>();
    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));
    for (const auto _action: {"client_join", "client_leave"}) {
        auto _transaction_id = to_string(_state->get_generator()());
        auto _client_id = to_string(_state->get_generator()());
        const boost::json::object _data = {
            {"action", _action}, {"transaction_id", _transaction_id},
            {"params", {{"session_id", "7"}, {"client_id", _client_id}}}
        };

        const auto _response = kernel(_state, _session, _data);

        LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
                 serialize(_response->get_data()));

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
        ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
        ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
        ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
                  "params session_id attribute must be uuid");

        ASSERT_TRUE(_response->get_data().contains("runtime"));
        ASSERT_TRUE(_response->get_data().at("runtime").is_number());
        ASSERT_TRUE(_response->get_data().at("runtime").as_int64() > 0);

        ASSERT_TRUE(_response->get_data().contains("timestamp"));
        ASSERT_TRUE(_response->get_data().at("timestamp").is_number());
        ASSERT_TRUE(_response->get_data().at("timestamp").as_int64() > 0);
        ASSERT_TRUE(
            _response->get_data().at("timestamp").as_int64() < std::chrono::system_clock::now().
            time_since_epoch().count());

        ASSERT_TRUE(_response->get_data().contains("transaction_id"));
        ASSERT_TRUE(_response->get_data().at("transaction_id").is_string());
        ASSERT_EQ(_response->get_data().at("transaction_id").as_string(), _transaction_id);
    }
}

TEST(validators_clients_validator_test, can_handle_empty_params_on_clients) {
    const auto _state = std::make_shared<aewt::state>();
    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));
    for (const auto _action: {"client_join", "client_leave"}) {
        auto _transaction_id = to_string(_state->get_generator()());
        auto _client_id = to_string(_state->get_generator()());
        const boost::json::object _data = {{"action", _action}, {"transaction_id", _transaction_id}};
        const auto _response = kernel(_state, _session, _data);

        LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
                 serialize(_response->get_data()));

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
        ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
        ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
        ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
                  "params attribute must be present");

        ASSERT_TRUE(_response->get_data().contains("runtime"));
        ASSERT_TRUE(_response->get_data().at("runtime").is_number());
        ASSERT_TRUE(_response->get_data().at("runtime").as_int64() > 0);

        ASSERT_TRUE(_response->get_data().contains("timestamp"));
        ASSERT_TRUE(_response->get_data().at("timestamp").is_number());
        ASSERT_TRUE(_response->get_data().at("timestamp").as_int64() > 0);
        ASSERT_TRUE(
            _response->get_data().at("timestamp").as_int64() < std::chrono::system_clock::now().
            time_since_epoch().count());

        ASSERT_TRUE(_response->get_data().contains("transaction_id"));
        ASSERT_TRUE(_response->get_data().at("transaction_id").is_string());
        ASSERT_EQ(_response->get_data().at("transaction_id").as_string(), _transaction_id);
    }
}

TEST(validators_clients_validator_test, can_handle_wrong_params_primivite_on_clients) {
    const auto _state = std::make_shared<aewt::state>();
    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));
    for (const auto _action: {"client_join", "client_leave"}) {
        auto _transaction_id = to_string(_state->get_generator()());
        auto _client_id = to_string(_state->get_generator()());
        const boost::json::object _data = {{"action", _action}, {"transaction_id", _transaction_id}, {"params", 7}};
        const auto _response = kernel(_state, _session, _data);

        LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
                 serialize(_response->get_data()));

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
        ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
        ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
        ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
                  "params attribute must be object");

        ASSERT_TRUE(_response->get_data().contains("runtime"));
        ASSERT_TRUE(_response->get_data().at("runtime").is_number());
        ASSERT_TRUE(_response->get_data().at("runtime").as_int64() > 0);

        ASSERT_TRUE(_response->get_data().contains("timestamp"));
        ASSERT_TRUE(_response->get_data().at("timestamp").is_number());
        ASSERT_TRUE(_response->get_data().at("timestamp").as_int64() > 0);
        ASSERT_TRUE(
            _response->get_data().at("timestamp").as_int64() < std::chrono::system_clock::now().
            time_since_epoch().count());

        ASSERT_TRUE(_response->get_data().contains("transaction_id"));
        ASSERT_TRUE(_response->get_data().at("transaction_id").is_string());
        ASSERT_EQ(_response->get_data().at("transaction_id").as_string(), _transaction_id);
    }
}
