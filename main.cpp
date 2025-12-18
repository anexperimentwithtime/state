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
#include <sentry.h>
#include <spdlog/spdlog.h>

#include <aewt/state.hpp>
#include <aewt/version.hpp>
#include <boost/version.hpp>

void sentry_start() {
  using namespace spdlog;
  using namespace aewt;
  using namespace version;

  info("sentry starting");
  sentry_options_t* _options = sentry_options_new();
  sentry_options_set_dsn(
      _options,
      dotenv::getenv(
          "SENTRY_DSN",
          "https://{username}@{token}.ingest.{zone}.sentry.io/{app_id}")
          .data());
  sentry_options_set_database_path(_options, ".sentry-native");
  const std::string _release_name =
      fmt::format("state@{}.{}.{}", get_major(), get_minor(), get_patch());
  sentry_options_set_release(_options, _release_name.data());
  sentry_options_set_debug(
      _options, dotenv::getenv("SENTRY_DEBUG", "OFF").data() == "ON");
  sentry_init(_options);
  info("sentry started");
}

void sentry_stop() {
  using namespace spdlog;

  info("sentry closing");
  sentry_close();
  info("sentry closed");
}

int main() {
  dotenv::init();
  using namespace std;
  using namespace aewt;
  using namespace spdlog;
  using namespace version;
  info("state version: {}.{}.{}", get_major(), get_minor(), get_patch());
  info("boost version: {}", BOOST_VERSION);

  sentry_start();
  auto _state = std::make_shared<state::instance>();
  sentry_stop();

  return 0;
}
