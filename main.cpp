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

#include <fmt/base.h>

#include <aewt/logger.hpp>
#include <aewt/state.hpp>
#include <aewt/config.hpp>
#include <aewt/server.hpp>
#include <aewt/session.hpp>

#include <aewt/version.hpp>

#include <boost/version.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

int main(const int argc, const char *argv[]) {
    boost::program_options::options_description _options("Options");
    auto _push_option = _options.add_options();

    _push_option("address", boost::program_options::value<std::string>()->default_value("0.0.0.0"));
    _push_option("threads", boost::program_options::value<unsigned short>()->default_value(4));
    _push_option("is_node", boost::program_options::value<bool>()->default_value(false));
    _push_option("sessions_port", boost::program_options::value<unsigned short>()->default_value(11000));
    _push_option("clients_port", boost::program_options::value<unsigned short>()->default_value(12000));
    _push_option("remote_address", boost::program_options::value<std::string>()->default_value("localhost"));
    _push_option("remote_sessions_port", boost::program_options::value<unsigned short>()->default_value(9000));
    _push_option("remote_clients_port", boost::program_options::value<unsigned short>()->default_value(10000));

    boost::program_options::variables_map _vm;
    store(parse_command_line(argc, argv, _options), _vm);

    const auto _server = std::make_shared<aewt::server>();

    _server->get_config()->address_ = _vm["address"].as<std::string>();
    _server->get_config()->threads_ = _vm["threads"].as<unsigned short>();
    _server->get_config()->is_node_ = _vm["is_node"].as<bool>();
    _server->get_config()->sessions_port_ = _vm["sessions_port"].as<unsigned short>();
    _server->get_config()->clients_port_ = _vm["clients_port"].as<unsigned short>();
    _server->get_config()->remote_address_ = _vm["remote_address"].as<std::string>();
    _server->get_config()->remote_sessions_port_ = _vm["remote_sessions_port"].as<unsigned short>();
    _server->get_config()->remote_clients_port_ = _vm["remote_clients_port"].as<unsigned short>();

    LOG_INFO("state version: {}.{}.{}", aewt::version::get_major(), aewt::version::get_minor(),
             aewt::version::get_patch());
    LOG_INFO("boost version: {}", BOOST_VERSION);
    LOG_INFO("configuration:");
    LOG_INFO("- threads: {}", _vm["threads"].as<unsigned short>());
    LOG_INFO("- address: {}", _vm["address"].as<std::string>());
    LOG_INFO("- sessions_port: {}", _vm["sessions_port"].as<unsigned short>());
    LOG_INFO("- clients_port: {}", _vm["clients_port"].as<unsigned short>());
    LOG_INFO("- is_node: {}", _vm["is_node"].as<bool>());
    LOG_INFO("- remote_address: {}", _vm["remote_address"].as<std::string>());
    LOG_INFO("- remote_sessions_port: {}", _vm["remote_sessions_port"].as<unsigned short>());
    LOG_INFO("- remote_clients_port: {}", _vm["remote_clients_port"].as<unsigned short>());

    _server->start();

    return 0;
}
