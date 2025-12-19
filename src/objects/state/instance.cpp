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
#include <aewt/state/session.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <ranges>

namespace aewt::state {

instance::instance()
    : id_(generator_()), created_at_(std::chrono::system_clock::now()) {
  spdlog::info("state instance {} allocated", to_string(id_));
}

instance::~instance() {
  spdlog::info("state instance {} released", to_string(id_));
}

boost::uuids::random_generator instance::get_generator() const {
  return generator_;
}

boost::uuids::uuid instance::get_id() const { return id_; }

std::chrono::system_clock::time_point instance::get_created_at() const {
  return created_at_;
}

std::vector<std::shared_ptr<session>> instance::get_sessions() const {
  std::shared_lock _lock(sessions_mutex_);
  std::vector<std::shared_ptr<session>> _result;
  _result.reserve(sessions_.size());

  for (auto& _session : sessions_ | std::views::values)
    _result.push_back(_session);

  return _result;
}

std::optional<std::shared_ptr<session>> instance::get_session(
    const boost::uuids::uuid id) const {
  std::shared_lock _lock(sessions_mutex_);

  const auto _iterator = sessions_.find(id);
  if (_iterator == sessions_.end()) {
    return std::nullopt;
  }
  return _iterator->second;
}

void instance::add_session(std::shared_ptr<session> session) {
  std::unique_lock _lock(sessions_mutex_);
  sessions_.emplace(session->get_id(), std::move(session));
}

void instance::remove_session(const boost::uuids::uuid id) {
  std::unique_lock _lock(sessions_mutex_);
  sessions_.erase(id);
}
}  // namespace aewt::state
