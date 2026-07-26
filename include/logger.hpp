#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

class Logger
{
public:

    template<typename... Args>
    static void log(const std::string& processName,
                    Args&&... args)
    {
#ifdef ENABLE_LOGGING
        std::lock_guard<std::mutex> lock(mutex_);

        std::ostringstream stream;

        stream << "[" << timestamp() << "] ";
        stream << "[" << processName << "] ";

        (stream << ... << std::forward<Args>(args));

        stream << '\n';

        std::cout << stream.str();
        std::cout.flush();
#else
        (void)processName;
        (void)sizeof...(args);
#endif
    }

private:

    static std::string timestamp();

    static std::mutex mutex_;
};

#ifdef ENABLE_LOGGING

#define LOG(PROCESS, ...) \
    Logger::log(PROCESS, __VA_ARGS__)

#else

#define LOG(PROCESS, ...)

#endif
