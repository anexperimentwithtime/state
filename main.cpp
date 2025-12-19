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

#include <spdlog/spdlog.h>

#include <aewt/state.hpp>
#include <aewt/version.hpp>
#include <boost/version.hpp>

std::string DEFAULT_SENTRY_DSN =
    "https://{username}@{token}.ingest.{zone}.sentry.io/{app_id}";
std::string DEFAULT_SENTRY_DEBUG = "OFF";

void sentry_start() {
  if (dotenv::getenv("SENTRY_ENABLED", DEFAULT_SENTRY_DEBUG).data() == "ON") {
#ifdef STATIC_ENABLED
    spdlog::info("sentry start isn't available on static");
#else
    spdlog::info("sentry starting");
    sentry_options_t* _options = sentry_options_new();
    sentry_options_set_dsn(
        _options, dotenv::getenv("SENTRY_DSN", DEFAULT_SENTRY_DSN).data());
    sentry_options_set_database_path(_options, ".sentry-native");
    const std::string _release_name =
        fmt::format("state@{}.{}.{}", aewt::version::get_major(),
                    aewt::version::get_minor(), aewt::version::get_patch());
    sentry_options_set_release(_options, _release_name.data());
    sentry_options_set_debug(
        _options,
        dotenv::getenv("SENTRY_DEBUG", DEFAULT_SENTRY_DEBUG).data() == "ON");
    sentry_init(_options);
    spdlog::info("sentry started");
#endif
  }
}

void sentry_stop() {
  if (dotenv::getenv("SENTRY_ENABLED", DEFAULT_SENTRY_DEBUG).data() == "ON") {
#ifdef STATIC_ENABLED
    spdlog::info("sentry close isn't available on static");
#else
    spdlog::info("sentry closing");
    sentry_close();
    spdlog::info("sentry closed");
#endif
  }
}

int main() {
  dotenv::init();
  spdlog::info("state version: {}.{}.{}", aewt::version::get_major(),
               aewt::version::get_minor(), aewt::version::get_patch());
  spdlog::info("boost version: {}", BOOST_VERSION);
  spdlog::info("environment variables:");
  spdlog::info("- SENTRY_DEBUG: {}",
               dotenv::getenv("SENTRY_DEBUG", DEFAULT_SENTRY_DEBUG));
  spdlog::info("- SENTRY_DSN: {}",
               dotenv::getenv("SENTRY_DSN", DEFAULT_SENTRY_DSN));

  sentry_start();
  auto _state = std::make_shared<aewt::state::instance>();
  sentry_stop();

  return 0;
}
