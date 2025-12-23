#pragma once

#ifndef AEWT_UTILS_HPP
#define AEWT_UTILS_HPP

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>

#define GET_PARAM_AS_ID(PARAMS, KEY) \
    boost::lexical_cast<boost::uuids::uuid>(std::string{ (PARAMS).at(KEY).as_string() })

#endif // AEWT_UTILS_HPP
