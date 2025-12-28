#pragma once

#ifndef AEWT_UTILS_HPP
#define AEWT_UTILS_HPP

#include <boost/json/object.hpp>
#include <boost/uuid/uuid.hpp>
#include <string>

namespace aewt {
    /**
     * Forward Request
     */
    struct request;

    /**
     * Forward Subscription
     */
    class subscription;

    /**
     * Forward Client
     */
    class client;

    /**
     * Forward Session
     */
    class session;

    /**
     * Forward State
     */
    class state;

    /**
     * Next
     *
     * @param request
     * @param status
     * @param data
     */
    void next(const request &request, const char *status, const boost::json::object &data = {});

    /**
     * Mark As Invalid
     *
     * @param request
     * @param field
     * @param argument
     */
    void mark_as_invalid(const request &request, const char *field, const char *argument);

    /**
     * Get Param As String
     *
     * @param params
     * @param field
     * @return string
     */
    std::string get_param_as_string(const boost::json::object &params, const char *field);

    /**
     * Get Params
     *
     * @param request
     * @return object
     */
    const boost::json::object &get_params(const request &request);

    /**
     * Get Param As Object
     *
     * @param params
     * @param field
     * @return object
     */
    const boost::json::object &get_param_as_object(const boost::json::object &params, const char *field);

    /**
     * Get Params As Value
     *
     * @param request
     * @return value
     */
    const boost::json::value &get_params_as_value(const request &request);

    /**
     * Get Params As ID
     *
     * @param params
     * @param field
     * @return uuid
     */
    boost::uuids::uuid get_param_as_id(const boost::json::object &params, const char *field);

    /**
     * Make Array Of IDs
     *
     * @param vector
     * @return array
     */
    boost::json::array make_array_of_ids(const std::vector<boost::uuids::uuid> &vector);

    /**
     * Make Channels Array Of Subscriptions
     *
     * @param subscriptions
     * @return array
     */
    boost::json::array make_channels_array_of_subscriptions(const std::vector<subscription> &subscriptions);

    /**
     * Make Array Of Clients
     *
     * @param clients
     * @return
     */
    boost::json::array make_array_of_clients(const std::vector<std::shared_ptr<client>> &clients);

    /**
     * Make Client Object
     * @param client
     * @param subscriptions
     * @return object
     */
    boost::json::object make_client_object(const std::shared_ptr<client> & client,
                                           const boost::json::array &subscriptions);

    /**
     * Make Session Object
     *
     * @param session
     * @return object
     */
    boost::json::object make_session_object(std::shared_ptr<session> session);

    /**
     * Make Broadcast Request Object
     *
     * @param request
     * @param session_id
     * @param client_id
     * @param payload
     * @return object
     */
    boost::json::object make_broadcast_request_object(const request &request, const boost::uuids::uuid &session_id,
                                                      const boost::uuids::uuid &client_id,
                                                      const boost::json::object &payload);

    /**
     * Get Status
     *
     * @param gate
     * @param on_true
     * @param on_false
     * @return char *
     */
    const char *get_status(bool gate, const char *on_true = "ok", const char *on_false = "no effect");

    /**
     * Add Client
     *
     * @param local
     * @param request
     * @param session_id
     * @param client_id
     * @param state
     * @return
     */
    bool add_client(bool local, const request &request, boost::uuids::uuid session_id, boost::uuids::uuid client_id, const std::shared_ptr<state> &state);
}

#endif // AEWT_UTILS_HPP
