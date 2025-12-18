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

#include <spdlog/spdlog.h>

#include <aewt/state.hpp>
#include <aewt/version.hpp>
#include <boost/version.hpp>

int main() {
  using namespace std;
  using namespace aewt;
  using namespace spdlog;
  {
    using namespace version;
    info("State version: {}.{}.{}", get_major(), get_minor(), get_patch());
  }
  info("Boost version: {}", BOOST_VERSION);
  auto _state = std::make_shared<state::instance>();
  return 0;
}
