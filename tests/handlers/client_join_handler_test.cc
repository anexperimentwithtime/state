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
#include <aewt/kernel_context.hpp>

#include <aewt/response.hpp>
#include <aewt/session.hpp>
#include <aewt/client.hpp>
#include <aewt/state.hpp>
#include <aewt/logger.hpp>

#include <boost/json/serialize.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../helpers.hpp"

using namespace aewt;

TEST(handlers_client_join_handler_test, can_handle) {
    boost::asio::io_context _io_context;

    const auto _state = std::make_shared<state>();

    const auto _remote_session = std::make_shared<session>(_state, boost::asio::ip::tcp::socket { _io_context });

    const auto _local_client = std::make_shared<client>(_state->get_id(), _state);

    const auto _remote_client = std::make_shared<client>(_remote_session->get_id(), _state);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "client_join"},
        {"transaction_id", to_string(_transaction_id)},
        {"params", {{"id", to_string(_remote_client->get_id())}}}
    };

    _state->add_session(_remote_session);

    const auto _response = kernel(_state, _data, on_session, _state->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());

    test_response_base_protocol_structure(_response, "success", "ok", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());

    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("count"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("count").is_number());

    _state->remove_session(_remote_session->get_id());
}

TEST(handlers_client_join_handler_test, can_handle_no_effect) {
    const auto _state = std::make_shared<state>();

    const auto _local_client = std::make_shared<client>(_state->get_id(), _state);

    _state->push_client(_local_client);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "client_join"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params", {
                {"id", to_string(_local_client->get_id())},
            }
        }
    };

    const auto _response = kernel(_state, _data, on_session, _state->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());

    test_response_base_protocol_structure(_response, "success", "no effect", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());

    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("count"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("count").is_number());

    _state->remove_client(_local_client->get_id());
}
