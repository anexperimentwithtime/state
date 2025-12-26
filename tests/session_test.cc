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

TEST(session_test, can_be_created) {
    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);

    const auto _state = std::make_shared<aewt::state>();
    const auto _session = std::make_shared<aewt::session>(_state, boost::uuids::random_generator()(),
                                                          std::move(_socket));

    _state->add_session(_session);

    ASSERT_TRUE(!_session->get_id().is_nil());
    ASSERT_TRUE(&_session->get_socket() == &_session->get_socket());

    _state->remove_session(_session->get_id());
}
