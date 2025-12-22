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
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

TEST(handlers_clients_handler_test, can_handle) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(boost::uuids::random_generator()(), std::move(_socket));

    auto _client_id = to_string(_state->get_generator()());
    auto _transaction_id = to_string(_state->get_generator()());

    _state->add_client(boost::lexical_cast<boost::uuids::uuid>(_client_id), _session->get_id());

    const boost::json::object _data = {{"action", "clients"}, {"transaction_id", _transaction_id}};

    const auto _response = kernel(_state, _session, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(), serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());
    ASSERT_TRUE(_response->get_data().contains("status"));
    ASSERT_TRUE(_response->get_data().at("status").is_string());
    ASSERT_EQ(_response->get_data().at("status").as_string(), "success");
    ASSERT_TRUE(_response->get_data().contains("message"));
    ASSERT_TRUE(_response->get_data().at("message").is_string());
    ASSERT_EQ(_response->get_data().at("message").as_string(), "ok");
    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());

    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("timestamp"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("timestamp").is_number());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("clients"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("clients").is_array());
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("clients").as_array().size() > 0);

    ASSERT_TRUE(_response->get_data().contains("transaction_id"));
    ASSERT_TRUE(_response->get_data().at("transaction_id").is_string());
    ASSERT_EQ(_response->get_data().at("transaction_id").as_string(), _transaction_id);
}