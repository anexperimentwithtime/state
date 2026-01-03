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

#include <aewt/server.hpp>
#include <aewt/logger.hpp>
#include <aewt/state.hpp>

class server_test : public testing::Test {
    std::unique_ptr<std::jthread> _remote_thread;
    std::unique_ptr<std::jthread> _local_thread;

protected:
    std::shared_ptr<aewt::server> _remote_server = std::make_shared<aewt::server>();
    std::shared_ptr<aewt::server> _local_server = std::make_shared<aewt::server>();

    void SetUp() override {
        _remote_thread = std::make_unique<std::jthread>([this]() {
            const auto &_config = _remote_server->get_config();
            _config->sessions_port_.store(0, std::memory_order_release);
            _config->clients_port_.store(0, std::memory_order_release);
            _config->repl_enabled = false;
            _config->threads_ = 4;
            _remote_server->start();
        });

        _local_thread = std::make_unique<std::jthread>([this]() {
            const auto &_config = _local_server->get_config();
            _config->sessions_port_.store(0, std::memory_order_release);
            _config->clients_port_.store(0, std::memory_order_release);
            _config->is_node_ = true;
            _config->threads_ = 4;
            _config->repl_enabled = false;

            while (_remote_server->get_config()->sessions_port_.load(std::memory_order_acquire) == 0 || _remote_server->
                   get_config()->clients_port_.load(std::memory_order_acquire) == 0) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                LOG_INFO("Waiting for remote ready ...");
            }

            _config->remote_clients_port_.store(
                _remote_server->get_config()->clients_port_.load(std::memory_order_acquire), std::memory_order_release);
            _config->remote_sessions_port_.store(
                _remote_server->get_config()->sessions_port_.load(std::memory_order_acquire),
                std::memory_order_release);

            _local_server->start();
        });

        while (_remote_server->get_config()->sessions_port_.load(std::memory_order_acquire) == 0 || _remote_server->
               get_config()->clients_port_.load(std::memory_order_acquire) == 0 ||
               _local_server->get_config()->sessions_port_.load(std::memory_order_acquire) == 0 || _local_server->
               get_config()->clients_port_.load(std::memory_order_acquire) == 0 || _local_server->get_config()->
               registered_.load(std::memory_order_acquire) == false) {
            LOG_INFO("Waiting 1 second for remote and local ready ...");
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void TearDown() override {
        LOG_INFO("Local server stop ...");
        _local_server->stop();
        LOG_INFO("Remote server stop ...");
        _remote_server->stop();
        while (!_local_server->get_state()->get_ioc().stopped() || !_remote_server->get_state()->get_ioc().stopped()) {
            LOG_INFO("Waiting for io stop ...");
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        _remote_server.reset();
        _local_server.reset();
    }
};
