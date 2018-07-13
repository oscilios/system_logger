#ifndef SYSTEM_LOGGER_LOGGER_H
#define SYSTEM_LOGGER_LOGGER_H

#include "system_logger/BasicLogger.h"

namespace system_logger
{
    using Logger = BasicLogger<>;
    using RtLogger =
        BasicLogger<char_type,
                    std::char_traits<char_type>,
                    memory::AllocatorWithInternalMemory<char_type, kStringStreamMemoryPoolBytes>>;

    template <typename LoggerT, typename... Args>
    void SYSLOG_DEBUG(LoggerT& logger, Args&&... args)
    {
        return logger(LogLevel::Debug, args...);
    }
    template <typename LoggerT, typename... Args>
    void SYSLOG_INFO(LoggerT& logger, Args&&... args)
    {
        return logger(LogLevel::Info, args...);
    }
    template <typename LoggerT, typename... Args>
    void SYSLOG_NOTICE(LoggerT& logger, Args&&... args)
    {
        return logger(LogLevel::Notice, args...);
    }
    template <typename LoggerT, typename... Args>
    void SYSLOG_WARNING(LoggerT& logger, Args&&... args)
    {
        return logger(LogLevel::Warning, args...);
    }
    template <typename LoggerT, typename... Args>
    void SYSLOG_ERROR(LoggerT& logger, Args&&... args)
    {
        return logger(LogLevel::Error, args...);
    }
    template <typename LoggerT, typename... Args>
    void SYSLOG_CRITICAL(LoggerT& logger, Args&&... args)
    {
        return logger(LogLevel::Critical, args...);
    }
    template <typename LoggerT, typename... Args>
    void SYSLOG_ALERT(LoggerT& logger, Args&&... args)
    {
        return logger(LogLevel::Alert, args...);
    }
    template <typename LoggerT, typename... Args>
    void SYSLOG_EMERGENCY(LoggerT& logger, Args&&... args)
    {
        return logger(LogLevel::Emergency, args...);
    }
}
#endif // SYSTEM_LOGGER_LOGGER_H
