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

#include <dotenv.h>
#include <fmt/base.h>

#ifdef STATIC_ENABLED
#else
#include <sentry.h>
#endif

#include <aewt/logger.hpp>
#include <aewt/state.hpp>
#include <aewt/session.hpp>
#include <aewt/session_listener.hpp>
#include <aewt/client_listener.hpp>

#include <aewt/version.hpp>

#include <boost/version.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/uuid/random_generator.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

std::string DEFAULT_SENTRY_DSN =
        "https://{username}@{token}.ingest.{zone}.sentry.io/{app_id}";
std::string DEFAULT_SENTRY_DEBUG = "OFF";

void sentry_start() {
    if (dotenv::getenv("SENTRY_ENABLED", DEFAULT_SENTRY_DEBUG).data() == "ON") {
#ifdef STATIC_ENABLED
        LOG_INFO("sentry start isn't available on static");
#else
        LOG_INFO("sentry starting");
        sentry_options_t *_options = sentry_options_new();
        sentry_options_set_dsn(_options, dotenv::getenv("SENTRY_DSN", DEFAULT_SENTRY_DSN).data());
        sentry_options_set_database_path(_options, ".sentry-native");
        const std::string _release_name =
                fmt::format("state@{}.{}.{}", aewt::version::get_major(),
                            aewt::version::get_minor(), aewt::version::get_patch());
        sentry_options_set_release(_options, _release_name.data());
        sentry_options_set_debug(
            _options,
            dotenv::getenv("SENTRY_DEBUG", DEFAULT_SENTRY_DEBUG).data() == "ON");
        sentry_init(_options);
        LOG_INFO("sentry started");
#endif
    }
}

void sentry_stop() {
    if (dotenv::getenv("SENTRY_ENABLED", DEFAULT_SENTRY_DEBUG).data() == "ON") {
#ifdef STATIC_ENABLED
        LOG_INFO("sentry close isn't available on static");
#else
        LOG_INFO("sentry closing");
        sentry_close();
        LOG_INFO("sentry closed");
#endif
    }
}

int main(const int argc, const char *argv[]) {
    dotenv::init();
    LOG_INFO("state version: {}.{}.{}", aewt::version::get_major(), aewt::version::get_minor(),
             aewt::version::get_patch());
    LOG_INFO("boost version: {}", BOOST_VERSION);
    LOG_INFO("environment variables:");
    LOG_INFO("- SENTRY_DEBUG: {}", dotenv::getenv("SENTRY_DEBUG", DEFAULT_SENTRY_DEBUG));
    LOG_INFO("- SENTRY_DSN: {}", dotenv::getenv("SENTRY_DSN", DEFAULT_SENTRY_DSN));

    sentry_start();
    boost::program_options::options_description _options("Options");
    auto _push_option = _options.add_options();

    boost::asio::io_context _ioc;
    boost::asio::ip::tcp::resolver _resolver{_ioc};
    auto _state = std::make_shared<aewt::state>();
    auto const _address = boost::asio::ip::make_address("0.0.0.0");
    auto const _threads = 4;

    _push_option("is_node", boost::program_options::value<bool>()->default_value(false));
    _push_option("sessions_port", boost::program_options::value<unsigned short>()->default_value(9000));
    _push_option("clients_port", boost::program_options::value<unsigned short>()->default_value(10000));

    boost::program_options::variables_map _vm;
    store(parse_command_line(argc, argv, _options), _vm);

    auto const _sessions_port = _vm["sessions_port"].as<unsigned short>();
    auto const _clients_port = _vm["clients_port"].as<unsigned short>();

    _state->set_ports(_sessions_port, _clients_port);

    auto _is_node = _vm["is_node"].as<bool>();

    if (_is_node) {
        auto _remote_host = dotenv::getenv("REMOTE_HOST", "127.0.0.1");
        auto _remote_port = dotenv::getenv("REMOTE_PORT", "10000");
        auto const _results = _resolver.resolve(_remote_host, _remote_port);

        const auto _remote_session = std::make_shared<aewt::session>(_state, boost::asio::ip::tcp::socket { _ioc });
        auto & _socket = _remote_session->get_socket();
        auto & _lowest_socket = _socket.next_layer().socket().lowest_layer();
        boost::asio::connect(_lowest_socket, _results);

        _remote_session->run(aewt::remote);

        _state->add_session(_remote_session);
    }

    std::make_shared<aewt::session_listener>(_ioc, boost::asio::ip::tcp::endpoint { _address, _sessions_port }, _state)
        ->start();
    std::make_shared<aewt::client_listener>(_ioc, boost::asio::ip::tcp::endpoint { _address, _clients_port }, _state)
        ->start();
    sentry_stop();

    std::vector<std::thread> _vector_of_threads;
    _vector_of_threads.reserve(_threads - 1);
    for(auto i = _threads - 1; i > 0; --i)
        _vector_of_threads.emplace_back(
        [&_ioc]
        {
            _ioc.run();
        });
    _ioc.run();

    return 0;
}
