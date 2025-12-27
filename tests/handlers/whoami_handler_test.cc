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
#include <aewt/client.hpp>
#include <aewt/state.hpp>
#include <aewt/logger.hpp>
#include <boost/json/serialize.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../helpers.hpp"

TEST(handlers_whoami_handler_test, can_handle) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "whoami"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"client_id", to_string(_local_client->get_id())},
                {"session_id", to_string(_current_session->get_id())}
            }
        }
    };

    _state->add_session(_current_session);

    const auto _response = kernel(_state, _data, _current_session->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());

    test_response_base_protocol_structure(_response, "success", "ok", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());

    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("id"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("id").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("id").as_string(), to_string(_current_session->get_id()));

    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("ip"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("ip").is_null());

    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("port"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("port").is_null());

    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("is_open"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("is_open").is_bool());
}
