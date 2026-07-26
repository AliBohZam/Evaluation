#include "logger.hpp"

#include <ctime>

std::mutex Logger::mutex_;

std::string Logger::timestamp()
{
    using namespace std::chrono;

    const auto now = system_clock::now();

    const auto milliseconds =
        duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) %
        1000;

    std::time_t currentTime =
        system_clock::to_time_t(now);

    std::tm localTime{};

#ifdef __APPLE__
    localtime_r(&currentTime, &localTime);
#else
    localtime_r(&currentTime, &localTime);
#endif

    std::ostringstream stream;

    stream << std::put_time(&localTime, "%H:%M:%S")
           << '.'
           << std::setw(3)
           << std::setfill('0')
           << milliseconds.count();

    return stream.str();
}
