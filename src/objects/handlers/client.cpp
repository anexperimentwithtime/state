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

#include <aewt/handlers/client.hpp>

#include <aewt/validators/client_id.hpp>

#include <aewt/response.hpp>
#include <aewt/state.hpp>
#include <aewt/session.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace aewt::handlers {
    void client(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response,
                const std::shared_ptr<state> &state, const std::shared_ptr<session> &session,
                const boost::json::object &data) {
        const auto _timestamp = std::chrono::system_clock::now();
        boost::ignore_unused(data);

        if (validators::client_id(transaction_id, response, data)) {
            const auto _client_id = boost::lexical_cast<boost::uuids::uuid>(std::string{
                data.at("params").as_object().at("client_id").as_string()
            });

            if (state->get_client_exists(_client_id)) {
                auto _client_subscriptions = state->get_client_subscriptions(_client_id);
                boost::json::array _subscriptions;
                _subscriptions.reserve(_client_subscriptions.size());
                for (const auto &_subscription: _client_subscriptions) {
                    _subscriptions.emplace_back(_subscription.channel_);
                }
                const boost::json::object _data = {
                    {"timestamp", _timestamp.time_since_epoch().count()},
                    {"id", to_string(_client_id)},
                    {"subscriptions", _subscriptions},
                };
                response->set_data(transaction_id, "ok", _data);
            } else {
                response->set_data(transaction_id, "no effect", {
                                  {"timestamp", _timestamp.time_since_epoch().count()},
                              });
            }
        }
    }
}
