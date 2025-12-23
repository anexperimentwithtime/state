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
}

#endif // AEWT_UTILS_HPP
