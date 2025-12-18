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

#include <aewt/state/instance.hpp>

namespace aewt::state {
using namespace spdlog;

instance::instance() : created_at_(system_clock::now()) {
  info("state instance allocated");
}

instance::~instance() { info("state instance released"); }

system_clock::time_point instance::get_created_at() const {
  return created_at_;
}
}  // namespace aewt::state
