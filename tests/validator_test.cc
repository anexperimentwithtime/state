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

#include <aewt/validator.hpp>

TEST(validator_test, validates_uuid) {
    ASSERT_FALSE(aewt::validator::is_uuid(""));
    ASSERT_FALSE(aewt::validator::is_uuid("7"));
    ASSERT_FALSE(aewt::validator::is_uuid("12345678-1234-1234-1234-1234567890"));
    ASSERT_FALSE(aewt::validator::is_uuid("12345678-1234-1234-1234-1234567890123"));
    ASSERT_FALSE(aewt::validator::is_uuid("12345678-1234-1234-1234-1234567890ZZ"));
    ASSERT_FALSE(aewt::validator::is_uuid("g2345678-1234-1234-1234-123456789012"));
    ASSERT_FALSE(aewt::validator::is_uuid("12345678-1234-1234-1234-12345678901G"));
    ASSERT_FALSE(aewt::validator::is_uuid("12345678-1234-1234-1234-12345"));
    ASSERT_FALSE(aewt::validator::is_uuid("12345678-1234-1234-1234-12345678901-"));
    ASSERT_FALSE(aewt::validator::is_uuid("12345678-1234-1234-1234_123456789012"));
    ASSERT_FALSE(aewt::validator::is_uuid("12345678-1234-1234-1234-12345678901Z"));
    ASSERT_TRUE(aewt::validator::is_uuid("12345678123412341234123456789012"));
    ASSERT_TRUE(aewt::validator::is_uuid("1a880b64-759e-4978-81c0-de7b90feedde"));
}
