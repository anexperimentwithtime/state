#pragma once

#ifndef AEWT_DISTRIBUTE_HPP
#define AEWT_DISTRIBUTE_HPP

#include <memory>
#include <boost/json/object.hpp>

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
     * @return size_t
     */
    std::size_t distribute_to_others(const std::shared_ptr<state> &state, const boost::json::object &data);
}

#endif // AEWT_DISTRIBUTE_HPP
