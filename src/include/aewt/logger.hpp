#pragma once

#ifndef AEWT_LOGGER_HPP
#define AEWT_LOGGER_HPP

#include <spdlog/spdlog.h>

#ifdef DEBUG_ENABLED
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#else
    #define LOG_INFO(...) ((void)0)
#endif

#endif // AEWT_LOGGER_HPP
