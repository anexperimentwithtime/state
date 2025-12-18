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
option(ENABLE_TESTS "Build tests" ON)
option(ENABLE_DEBUG "Enable debug" ON)
option(ENABLE_STATIC_LINKING "Enable static linking" OFF)
option(ENABLE_NATIVE_OPTIMIZATION "Enable native CPU optimization" OFF)
option(ENABLE_CI "Enable CI settings" OFF)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g --coverage -fprofile-arcs -ftest-coverage")

if (ENABLE_DEBUG)
    add_definitions(-DDEBUG_ENABLED)
endif ()

if (ENABLE_STATIC_LINKING)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(CURL_USE_STATIC_LIBS ON)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_STATIC_RUNTIME ON)
    set(BOOST_ALL_NO_LIB ON)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
    set(OPENSSL_USE_STATIC_LIBS TRUE)

    if (ENABLE_CI)
        set(OPENSSL_CRYPTO_LIBRARY "/usr/lib/libcrypto.a")
        set(OPENSSL_SSL_LIBRARY "/usr/lib/libssl.a")
    else ()
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
            set(ARCH_LIB_DIR "/usr/lib/aarch64-linux-gnu")
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
            set(ARCH_LIB_DIR "/usr/lib/x86_64-linux-gnu")
        else()
            message(FATAL_ERROR "Arquitectura no soportada: ${CMAKE_SYSTEM_PROCESSOR}")
        endif()
        set(OPENSSL_CRYPTO_LIBRARY "${ARCH_LIB_DIR}/libcrypto.a")
        set(OPENSSL_SSL_LIBRARY "${ARCH_LIB_DIR}/libssl.a")
    endif ()

    set(OpenSSL_USE_STATIC_LIBS TRUE)

    if (ENABLE_NATIVE_OPTIMIZATION)
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -march=native -mtune=native -g0 -static -static-libgcc")
        set(CMAKE_C_FLAGS_RELEASE "-O3 -flto -march=native -mtune=native -g0 -static -static-libgcc")
    else ()
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -g0 -static -static-libgcc")
        set(CMAKE_C_FLAGS_RELEASE "-O3 -flto -g0 -static -static-libgcc")
    endif ()
else ()
    if (ENABLE_NATIVE_OPTIMIZATION)
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -march=native -mtune=native -g0")
        set(CMAKE_C_FLAGS_RELEASE "-O3 -flto -march=native -mtune=native -g0")
    else ()
        set(CMAKE_CXX_FLAGS_RELEASE "-O3")
        set(CMAKE_C_FLAGS_RELEASE "-O3")
    endif ()
endif ()

if (ENABLE_DEBUG)
    add_definitions(-DDEBUG_ENABLED)
endif ()


find_package(Boost REQUIRED COMPONENTS program_options json charconv)

find_package(
        Boost
        REQUIRED
        COMPONENTS
        program_options
)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog REQUIRED)
find_package(sentry REQUIRED)
find_package(laserpants_dotenv REQUIRED)
find_package(fmt REQUIRED)

file(GLOB_RECURSE STATE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/objects/*.cpp")

include_directories(src/include)

add_library(objects OBJECT ${STATE_SOURCES})
target_link_libraries(objects PUBLIC ${Boost_LIBRARIES} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} spdlog::spdlog sentry::sentry fmt::fmt laserpants::dotenv)

add_executable(state main.cpp)
target_link_libraries(state PUBLIC objects)

if (ENABLE_STATIC_LINKING)
    target_compile_options(state PRIVATE -static -static-libgcc -static-libstdc++)
    target_link_options(state PRIVATE -static -static-libgcc -static-libstdc++)
endif()

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

    add_executable(tests ${STATE_TESTS})
    target_link_libraries(tests PUBLIC objects)

    target_link_libraries(tests
            PRIVATE
            GTest::gtest_main
    )

    if (ENABLE_STATIC_LINKING)
        target_compile_options(tests PRIVATE -static -static-libgcc -static-libstdc++)
        target_link_options(tests PRIVATE -static -static-libgcc -static-libstdc++)
    endif()

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()