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

#include <aewt/session.hpp>
#include <aewt/state.hpp>

#include <boost/uuid/random_generator.hpp>

TEST(state_test, can_be_created) {
    const auto _state = std::make_shared<aewt::state>();
    ASSERT_TRUE(!boost::uuids::random_generator()().is_nil());
    ASSERT_TRUE(!_state->get_id().is_nil());
    ASSERT_TRUE(boost::uuids::random_generator()() != _state->get_id());
    ASSERT_TRUE(std::chrono::system_clock::now() > _state->get_created_at());
}

TEST(state_test, can_contains_sessions) {
    const auto _state = std::make_shared<aewt::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<aewt::session>(_state, boost::asio::ip::tcp::socket{ _io_context });
    ASSERT_EQ(_state->get_session(_session->get_id()), std::nullopt);
    ASSERT_EQ(_state->get_sessions().size(), 0);
    _state->add_session(_session);
    ASSERT_EQ(_state->get_sessions().size(), 1);
    ASSERT_TRUE(_state->get_session(_session->get_id()).has_value());
    ASSERT_EQ(_state->get_session(_session->get_id()).value()->get_id(), _session->get_id());
    _state->remove_session(_session->get_id());
    ASSERT_EQ(_state->get_sessions().size(), 0);
    ASSERT_EQ(_state->get_session(_session->get_id()), std::nullopt);
}
