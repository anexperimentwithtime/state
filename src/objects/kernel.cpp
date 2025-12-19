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

#include <fmt/format.h>

#include <aewt/kernel.hpp>
#include <aewt/response.hpp>
#include <aewt/session.hpp>
#include <aewt/state.hpp>
#include <aewt/validator.hpp>
#include <boost/core/ignore_unused.hpp>

namespace aewt {
    std::shared_ptr<response> kernel(const std::shared_ptr<state>& state,
                                     const std::shared_ptr<session>& session,
                                     boost::json::object data) {
        boost::ignore_unused(state, session, data);
        auto _response = std::make_shared<response>();
        if (const validator _validator(data); _validator.get_passed()) {
            const std::string _action{data.at("action").as_string()};

            if (_action == "ping") {
                _response->set_data(
                    "pong",
                    {
                        {
                            "timestamp",
                            std::chrono::system_clock::now().time_since_epoch().count()
                        },
                    });
            } else {
                _response->mark_as_failed(
                    "unprocessable entity",
                    {
                        {
                            "action",
                            fmt::format("action attribute isn't implemented", _action)
                        }
                    });
            }
        } else {
            _response->mark_as_failed("unprocessable entity", _validator.get_bag());
        }
        _response->mark_as_processed();
        return _response;
    }
} // namespace aewt
