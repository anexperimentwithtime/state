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
#include <aewt/logger.hpp>

#include <boost/uuid/uuid_io.hpp>
#include <boost/json/serialize.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/lexical_cast.hpp>

namespace aewt {
    using validation_rules = std::vector<std::tuple<
        std::function<bool(const boost::json::object &body)>,
        std::map<std::string, std::string>
    > >;

    bool validate(const validation_rules &rules, const std::shared_ptr<response> &response,
                  const boost::uuids::uuid transaction_id, const boost::json::object &data) {
        for (const auto &[_rule, _bag]: rules) {
            if (_rule(data)) {
                response->mark_as_failed(transaction_id, "unprocessable entity", _bag);
                return false;
            }
        }
        return true;
    }

    void handle_ping(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response) {
        response->set_data(transaction_id, "pong", {
                               {"transaction_id", to_string(transaction_id)},
                               {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()}
                           });
    }

    void handle_whoami(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response,
                       const std::shared_ptr<session> &session) {
        boost::json::object _data = {
            {"transaction_id", to_string(transaction_id)},
            {"id", to_string(session->get_id())},
            {"is_open", session->get_socket().is_open()},
        };

        if (session->get_socket().is_open()) {
            _data["ip"] = session->get_socket().remote_endpoint().address().to_string();
            _data["port"] = session->get_socket().remote_endpoint().port();
        } else {
            _data["ip"] = nullptr;
            _data["port"] = nullptr;
        }

        response->set_data(transaction_id, "im", _data);
    }

    void handle_unimplemented(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response) {
        response->mark_as_failed(transaction_id, "unprocessable entity", {
                                     {"action", "action attribute isn't implemented"}
                                 });
    }

    bool validate_subscribe_and_unsubscribe_payload(const boost::uuids::uuid transaction_id,
                                                    const std::shared_ptr<response> &response,
                                                    const boost::json::object &data) {
        if (!data.contains("params")) {
            response->mark_as_failed(transaction_id, "unprocessable entity", {{"params", "params attribute must be present"}});
            return false;
        }

        const boost::json::value _params = data.at("params");
        if (!_params.is_object()) {
            response->mark_as_failed(transaction_id, "unprocessable entity", {{"params", "params attribute must be object"}});
            return false;
        }

        const boost::json::object _params_object = _params.as_object();
        if (!_params_object.contains("channel")) {
            response->mark_as_failed(transaction_id, "unprocessable entity", {{"params", "params channel attribute must be present"}});
            return false;
        }

        const boost::json::value _channel = _params_object.at("channel");
        if (!_channel.is_string()) {
            response->mark_as_failed(transaction_id, "unprocessable entity", {{"params", "params channel attribute must be string"}});
            return false;
        }

        if (!_params_object.contains("client_id")) {
            response->mark_as_failed(transaction_id, "unprocessable entity", {{"params", "params client_id attribute must be present"}});
            return false;
        }

        const boost::json::value _client_id = _params_object.at("client_id");
        if (!_client_id.is_string()) {
            response->mark_as_failed(transaction_id, "unprocessable entity", {{"params", "params client_id attribute must be string"}});
            return false;
        }

        if (!validator::is_uuid(_client_id.as_string().c_str())) {
            response->mark_as_failed(transaction_id, "unprocessable entity", {{"params", "params client_id attribute must be uuid"}});
            return false;
        }

        return true;
    }

    bool validate_is_subscribed_payload(const boost::uuids::uuid transaction_id,
                                        const std::shared_ptr<response> &response,
                                        const boost::json::object &data) {
        const validation_rules _rules
        {
            {
                [](const boost::json::object &body) { return !body.contains("params"); },
                {{"params", "params attribute must be present"}}
            },
            {
                [](const boost::json::object &body) { return !body.at("params").is_object(); },
                {{"params", "params attribute must be object"}}
            },
            {
                [](const boost::json::object &body) { return !body.at("params").as_object().contains("channel"); },
                {{"params", "params channel attribute must be present"}}
            },
            {
                [](const boost::json::object &body) {
                    return !body.at("params").as_object().at("channel").is_string();
                },
                {{"params", "params channel attribute must be string"}}
            },
            {
                [](const boost::json::object &body) { return !body.at("params").as_object().contains("client_id"); },
                {{"params", "params client_id attribute must be present"}}
            },
            {
                [](const boost::json::object &body) {
                    return !body.at("params").as_object().at("client_id").is_string();
                },
                {{"params", "params client_id attribute must be string"}}
            },
            {
                [](const boost::json::object &body) {
                    return !validator::is_uuid(body.at("params").as_object().at("client_id").as_string().c_str());
                },
                {{"params", "params client_id attribute must be uuid"}}
            },
            {
                [](const boost::json::object &body) { return !body.at("params").as_object().contains("session_id"); },
                {{"params", "params session_id attribute must be present"}}
            },
            {
                [](const boost::json::object &body) {
                    return !body.at("params").as_object().at("session_id").is_string();
                },
                {{"params", "params session_id attribute must be string"}}
            },
            {
                [](const boost::json::object &body) {
                    return !validator::is_uuid(body.at("params").as_object().at("session_id").as_string().c_str());
                },
                {{"params", "params session_id attribute must be uuid"}}
            },
        };

        return validate(_rules, response, transaction_id, data);
    }

    void handle_subscribe(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response,
                          const std::shared_ptr<state> &state,
                          const std::shared_ptr<session> &session, const boost::json::object &data) {
        if (validate_subscribe_and_unsubscribe_payload(transaction_id, response, data)) {
            const auto client_id = boost::lexical_cast<boost::uuids::uuid>(
                std::string{data.at("params").as_object().at("client_id").as_string()});
            const std::string channel{data.at("params").as_object().at("channel").as_string()};

            const auto _timestamp = std::chrono::system_clock::now();
            const bool _success = state->subscribe(session->get_id(), client_id, channel);
            response->set_data(transaction_id, _success ? "ok" : "no effect", {
                                   {"timestamp", _timestamp.time_since_epoch().count()}
                               });
        }
    }

    void handle_is_subscribed(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response,
                              const std::shared_ptr<state> &state,
                              const std::shared_ptr<session> &session, const boost::json::object &data) {
        if (validate_is_subscribed_payload(transaction_id, response, data)) {
            const auto client_id = boost::lexical_cast<boost::uuids::uuid>(
                std::string{data.at("params").as_object().at("client_id").as_string()});
            const auto session_id = boost::lexical_cast<boost::uuids::uuid>(
                std::string{data.at("params").as_object().at("session_id").as_string()});
            const std::string channel{data.at("params").as_object().at("channel").as_string()};
            const auto _timestamp = std::chrono::system_clock::now();
            const bool _success = state->is_subscribed(session_id, client_id, channel);
            response->set_data(transaction_id, _success ? "yes" : "no", {
                                   {"timestamp", _timestamp.time_since_epoch().count()}
                               });
        }
    }

    void handle_unsubscribe(const boost::uuids::uuid transaction_id, const std::shared_ptr<response> &response,
                            const std::shared_ptr<state> &state,
                            const std::shared_ptr<session> &session, const boost::json::object &data) {
        if (validate_subscribe_and_unsubscribe_payload(transaction_id, response, data)) {
            const auto client_id = boost::lexical_cast<boost::uuids::uuid>(
                std::string{data.at("params").as_object().at("client_id").as_string()});
            const std::string channel{data.at("params").as_object().at("channel").as_string()};

            const auto _timestamp = std::chrono::system_clock::now();
            const bool _success = state->unsubscribe(session->get_id(), client_id, channel);
            response->set_data(transaction_id, _success ? "ok" : "no effect", {
                                   {"timestamp", _timestamp.time_since_epoch().count()}
                               });
        }
    }

    bool validate_unsubscribe_all_client_payload(const boost::uuids::uuid transaction_id,
                                                 const std::shared_ptr<response> &response,
                                                 const boost::json::object &data) {
        const validation_rules _rules
        {
            {
                [](const boost::json::object &body) { return !body.contains("params"); },
                {{"params", "params attribute must be present"}}
            },
            {
                [](const boost::json::object &body) { return !body.at("params").is_object(); },
                {{"params", "params attribute must be object"}}
            },
            {
                [](const boost::json::object &body) { return !body.at("params").as_object().contains("client_id"); },
                {{"params", "params client_id attribute must be present"}}
            },
            {
                [](const boost::json::object &body) {
                    return !body.at("params").as_object().at("client_id").is_string();
                },
                {{"params", "params client_id attribute must be string"}}
            },
            {
                [](const boost::json::object &body) {
                    return !validator::is_uuid(body.at("params").as_object().at("client_id").as_string().c_str());
                },
                {{"params", "params client_id attribute must be uuid"}}
            },
        };

        return validate(_rules, response, transaction_id, data);
    }

    void handle_unsubscribe_all_client(const boost::uuids::uuid transaction_id,
                                       const std::shared_ptr<response> &response, const std::shared_ptr<state> &state,
                                       const std::shared_ptr<session> &session, const boost::json::object &data) {
        if (validate_unsubscribe_all_client_payload(transaction_id, response, data)) {
            const auto client_id = boost::lexical_cast<boost::uuids::uuid>(
                std::string{data.at("params").as_object().at("client_id").as_string()});

            const auto _timestamp = std::chrono::system_clock::now();
            const std::size_t _count = state->unsubscribe_all_client(client_id);
            response->set_data(transaction_id, _count > 0 ? "ok" : "no effect", {
                                   {"timestamp", _timestamp.time_since_epoch().count()},
                                   {"count", _count}
                               });
        }
    }

    bool validate_unsubscribe_all_session_payload(const boost::uuids::uuid transaction_id,
                                                  const std::shared_ptr<response> &response,
                                                  const boost::json::object &data) {
        const validation_rules _rules
        {
            {
                [](const boost::json::object &body) { return !body.contains("params"); },
                {{"params", "params attribute must be present"}}
            },
            {
                [](const boost::json::object &body) { return !body.at("params").is_object(); },
                {{"params", "params attribute must be object"}}
            },
            {
                [](const boost::json::object &body) { return !body.at("params").as_object().contains("session_id"); },
                {{"params", "params session_id attribute must be present"}}
            },
            {
                [](const boost::json::object &body) {
                    return !body.at("params").as_object().at("session_id").is_string();
                },
                {{"params", "params session_id attribute must be string"}}
            },
            {
                [](const boost::json::object &body) {
                    return !validator::is_uuid(body.at("params").as_object().at("session_id").as_string().c_str());
                },
                {{"params", "params session_id attribute must be uuid"}}
            },
        };

        return validate(_rules, response, transaction_id, data);
    }

    void handle_unsubscribe_all_session(const boost::uuids::uuid transaction_id,
                                        const std::shared_ptr<response> &response, const std::shared_ptr<state> &state,
                                        const std::shared_ptr<session> &session, const boost::json::object &data) {
        if (validate_unsubscribe_all_session_payload(transaction_id, response, data)) {
            const auto session_id = boost::lexical_cast<boost::uuids::uuid>(
                std::string{data.at("params").as_object().at("session_id").as_string()});

            const auto _timestamp = std::chrono::system_clock::now();
            const std::size_t _count = state->unsubscribe_all_session(session_id);
            response->set_data(transaction_id, _count > 0 ? "ok" : "no effect", {
                                   {"timestamp", _timestamp.time_since_epoch().count()},
                                   {"count", _count}
                               });
        }
    }

    std::shared_ptr<response> kernel(const std::shared_ptr<state> &state,
                                     const std::shared_ptr<session> &session,
                                     boost::json::object data) {
        boost::ignore_unused(state);

        auto _response = std::make_shared<response>();
        if (const validator _validator(data); _validator.get_passed()) {
            const auto transaction_id = boost::lexical_cast<boost::uuids::uuid>(std::string{
                data.at("transaction_id").as_string()
            });
            if (const std::string _action{data.at("action").as_string()}; _action == "ping") {
                handle_ping(transaction_id, _response);
            } else if (_action == "subscribe") {
                handle_subscribe(transaction_id, _response, state, session, data);
            } else if (_action == "is_subscribed") {
                handle_is_subscribed(transaction_id, _response, state, session, data);
            } else if (_action == "unsubscribe") {
                handle_unsubscribe(transaction_id, _response, state, session, data);
            } else if (_action == "unsubscribe_all_client") {
                handle_unsubscribe_all_client(transaction_id, _response, state, session, data);
            } else if (_action == "unsubscribe_all_session") {
                handle_unsubscribe_all_session(transaction_id, _response, state, session, data);
            } else if (_action == "whoami") {
                handle_whoami(transaction_id, _response, session);
            } else {
                handle_unimplemented(transaction_id, _response);
            }
        } else {
            if (data.contains("transaction_id") && data.at("transaction_id").is_string() && validator::is_uuid(
                data.at("transaction_id").as_string().c_str())) {
                _response->mark_as_failed(
                    boost::lexical_cast<boost::uuids::uuid>(std::string{data.at("transaction_id").as_string()}),
                    "unprocessable entity", _validator.get_bag());
            } else {
                _response->mark_as_failed(state->get_generator()(), "unprocessable entity", _validator.get_bag());
            }
        }
        _response->mark_as_processed();
        return _response;
    }
} // namespace aewt
