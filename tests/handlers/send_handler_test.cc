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
#include <boost/lexical_cast.hpp>
#include <boost/json/serialize.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../helpers.hpp"

TEST(handlers_send_handler_test, can_handle) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    boost::asio::ip::tcp::socket _remote_socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _remote_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                 std::move(_remote_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);
    const auto _remote_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                               _current_session->get_id(), false);

    _state->add_session(_current_session);
    _state->add_session(_remote_session);
    _state->add_client(_local_client);
    _state->add_client(_remote_client);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"sender_id", to_string(_local_client->get_id())}, {"receiver_id", to_string(_remote_client->get_id())},
                {"session_id", to_string(_current_session->get_id())},
                {"payload", boost::json::object({{"message", "EHLO"}})}
            }
        }
    };

    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());

    test_response_base_protocol_structure(_response, "success", "ok", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());

    _state->remove_session(_current_session->get_id());
    _state->remove_session(_remote_session->get_id());
    _state->remove_client(_local_client->get_id());
    _state->remove_client(_remote_client->get_id());
}

TEST(handlers_send_handler_test, can_handle_no_effect) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    boost::asio::ip::tcp::socket _remote_socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _remote_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                 std::move(_remote_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);
    const auto _remote_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                               _current_session->get_id(), false);

    _state->add_session(_current_session);
    _state->add_client(_local_client);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"sender_id", to_string(_local_client->get_id())}, {"receiver_id", to_string(_remote_client->get_id())},
                {"session_id", to_string(_current_session->get_id())},
                {"payload", boost::json::object({{"message", "EHLO"}})}
            }
        }
    };

    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());

    test_response_base_protocol_structure(_response, "success", "no effect", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());

    _state->remove_client(_local_client->get_id());
}

TEST(handlers_send_handler_test, can_handle_send_on_empty_data_params) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {{"action", "send"}, {"transaction_id", to_string(_transaction_id)}};

    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
              "params attribute must be present");
}

TEST(handlers_send_handler_test, can_handle_send_on_wrong_data_params_primitive) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)}, {"params", 7}
    };
    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
              "params attribute must be object");
}

TEST(handlers_send_handler_test, can_handle_send_on_empty_data_params_session_id) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);
    const auto _remote_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                               _current_session->get_id(), false);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"sender_id", to_string(_local_client->get_id())}, {"receiver_id", to_string(_remote_client->get_id())},
                {"payload", boost::json::object({{"message", "EHLO"}})}
            }
        }
    };

    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
              "params session_id attribute must be present");
}

TEST(handlers_send_handler_test, can_handle_send_on_wrong_data_params_session_id_primitive) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);
    const auto _remote_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                               _current_session->get_id(), false);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"sender_id", to_string(_local_client->get_id())}, {"receiver_id", to_string(_remote_client->get_id())},
                {"session_id", 7},
                {"payload", boost::json::object({{"message", "EHLO"}})}
            }
        }
    };
    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
              "params session_id attribute must be string");
}

TEST(handlers_send_handler_test, can_handle_send_on_wrong_data_params_session_id_type) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);
    const auto _remote_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                               _current_session->get_id(), false);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"sender_id", to_string(_local_client->get_id())}, {"receiver_id", to_string(_remote_client->get_id())},
                {"session_id", "7"},
                {"payload", boost::json::object({{"message", "EHLO"}})}
            }
        }
    };
    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
              "params session_id attribute must be uuid");
}

TEST(handlers_send_handler_test, can_handle_send_on_empty_data_params_sender_id) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);
    const auto _remote_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                               _current_session->get_id(), false);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"receiver_id", to_string(_remote_client->get_id())},
                {"session_id", to_string(_current_session->get_id())},
                {"payload", boost::json::object({{"message", "EHLO"}})}
            }
        }
    };

    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
              "params sender_id attribute must be present");
}

TEST(handlers_send_handler_test, can_handle_send_on_wrong_data_params_sender_id_primitive) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);
    const auto _remote_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                               _current_session->get_id(), false);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"sender_id", 7}, {"receiver_id", to_string(_remote_client->get_id())},
                {"session_id", to_string(_current_session->get_id())},
                {"payload", boost::json::object({{"message", "EHLO"}})}
            }
        }
    };
    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
              "params sender_id attribute must be string");
}

TEST(handlers_send_handler_test, can_handle_send_on_wrong_data_params_sender_id_type) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);
    const auto _remote_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                               _current_session->get_id(), false);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"sender_id", "7"}, {"receiver_id", to_string(_remote_client->get_id())},
                {"session_id", to_string(_current_session->get_id())},
                {"payload", boost::json::object({{"message", "EHLO"}})}
            }
        }
    };
    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
              "params sender_id attribute must be uuid");
}

TEST(handlers_send_handler_test, can_handle_send_on_empty_data_params_receiver_id) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"sender_id", to_string(_local_client->get_id())},
                {"session_id", to_string(_current_session->get_id())},
                {"payload", boost::json::object({{"message", "EHLO"}})}
            }
        }
    };
    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
              "params receiver_id attribute must be present");
}

TEST(handlers_send_handler_test, can_handle_send_on_wrong_data_params_receiver_id_primitive) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"sender_id", to_string(_local_client->get_id())}, {"receiver_id", 7},
                {"session_id", to_string(_current_session->get_id())},
                {"payload", boost::json::object({{"message", "EHLO"}})}
            }
        }
    };
    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
              "params receiver_id attribute must be string");
}

TEST(handlers_send_handler_test, can_handle_send_on_wrong_data_params_receiver_id_type) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"sender_id", to_string(_local_client->get_id())}, {"receiver_id", "7"},
                {"session_id", to_string(_current_session->get_id())},
                {"payload", boost::json::object({{"message", "EHLO"}})}
            }
        }
    };
    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
              "params receiver_id attribute must be uuid");
}

TEST(handlers_send_handler_test, can_handle_send_on_empty_data_params_payload) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);
    const auto _remote_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                               _current_session->get_id(), false);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"sender_id", to_string(_local_client->get_id())}, {"receiver_id", to_string(_remote_client->get_id())},
                {"session_id", to_string(_current_session->get_id())}
            }
        }
    };
    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
              "params payload attribute must be present");
}

TEST(handlers_send_handler_test, can_handle_send_on_wrong_data_params_payload_primitive) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _current_session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                                  std::move(_socket));
    const auto _local_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                              _current_session->get_id(), true);
    const auto _remote_client = std::make_shared<aewt::client>(boost::uuids::random_generator()(),
                                                               _current_session->get_id(), false);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"}, {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"sender_id", to_string(_local_client->get_id())}, {"receiver_id", to_string(_remote_client->get_id())},
                {"session_id", to_string(_current_session->get_id())},
                {"payload", 7}
            }
        }
    };
    const auto _response = kernel(_state, _current_session, _local_client, _data);

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
              "params payload attribute must be object");
}
