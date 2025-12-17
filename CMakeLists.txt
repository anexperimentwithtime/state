# Copyright (C) 2025 Ian Torres <iantorres@outlook.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.30)
project(state)

set(CMAKE_CXX_STANDARD 23)
option(ENABLE_TESTS "Build tests cases" ON)

find_package(Boost REQUIRED COMPONENTS program_options json charconv)

find_package(
        Boost
        REQUIRED
        COMPONENTS
        program_options
)

file(GLOB_RECURSE STATE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/objects/*.cpp")

include_directories(src/include)

add_executable(state main.cpp ${STATE_SOURCES})
target_link_libraries(state ${Boost_LIBRARIES})

if (ENABLE_TESTS)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    file(GLOB_RECURSE STATE_TESTS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc")

    add_executable(tests ${STATE_SOURCES} ${STATE_TESTS})

    target_link_libraries(tests
            PRIVATE
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()