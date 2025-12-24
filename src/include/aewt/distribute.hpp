#pragma once

#ifndef AEWT_DISTRIBUTE_HPP
#define AEWT_DISTRIBUTE_HPP

#include <memory>
#include <boost/json/object.hpp>
#include <boost/uuid/uuid.hpp>

namespace aewt {
    /**
     * Forward State
     */
    class state;

    /**
     * Distribute To Others
     *
     * @param state
     * @param data
     * @param session_id
     * @return size_t
     */
    std::size_t distribute_to_others(const std::shared_ptr<state> &state, const boost::json::object &data, boost::uuids::uuid session_id);
}

#endif // AEWT_DISTRIBUTE_HPP
