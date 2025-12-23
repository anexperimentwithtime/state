#pragma once

#ifndef AEWT_UTILS_HPP
#define AEWT_UTILS_HPP

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/json/object.hpp>
#include <string>

namespace aewt {
    struct request;

    void next(const request & request, const char * status, const boost::json::object & data = {});

    void mark_as_invalid(const request & request, const char * field, const char * argument);

    std::string get_param_as_string(const boost::json::object & params, const char * field);

    const boost::json::object &get_params(const request & request);

    const boost::json::object &get_param_as_object(const boost::json::object & params, const char * field);

    const boost::json::value &get_params_as_value(const request & request);

    boost::uuids::uuid get_param_as_id(const boost::json::object & params, const char * field);
}

#endif // AEWT_UTILS_HPP
