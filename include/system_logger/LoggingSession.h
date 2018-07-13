#ifndef SYSTEM_LOGGER_LOGGING_SESSION_H
#define SYSTEM_LOGGER_LOGGING_SESSION_H

#include "system_logger/Semaphore.h"
#include "system_logger/StringRingBuffer.h"

#include <atomic>
#include <memory>
#include <mutex> // once_flag
#include <string>
#include <thread>

#ifdef SYSTEM_LOGGER_DEFINITIONS_HEADER
#include SYSTEM_LOGGER_DEFINITIONS_HEADER
#else
namespace system_logger
{
    constexpr size_t kMemoryPoolBytes             = 16384;
    constexpr size_t kStringBufferSize            = 256;
    constexpr size_t kStringStreamMemoryPoolBytes = 1024;
    using char_type = char;
}
#endif // SYSTEM_LOGGER_DEFINITIONS_HEADER

namespace system_logger
{
    enum class Option : unsigned;
    enum class LogLevel : char;
    Option operator|(Option lhs, Option rhs);
    class LoggingSession;
}

enum class system_logger::Option : unsigned
{
    PID    = 1,
    CONS   = 2,
    ODELAY = 4,
    NDELAY = 8,
    NOWAIT = 16,
    PERROR = 32
};

enum class system_logger::LogLevel : char
{
    Emergency = '0',
    Alert     = '1',
    Critical  = '2',
    Error     = '3',
    Warning   = '4',
    Notice    = '5',
    Info      = '6',
    Debug     = '7',
};

class system_logger::LoggingSession final
{
    using StringBuffer = threadsafe::StringRingBuffer<char_type, kStringBufferSize, kMemoryPoolBytes>;

    std::once_flag m_initFlag;
    static LogLevel m_loglevel;
    static int m_syslogLevel;
    std::thread m_thread;
    threadsafe::Semaphore m_semaphore;
    StringBuffer m_stringbuffer;
    std::atomic<bool> m_done{false};

    LoggingSession() = default;
    void pop();

public:
    LoggingSession(const LoggingSession&)     = delete;
    LoggingSession(LoggingSession&&) noexcept = delete;
    ~LoggingSession();
    LoggingSession& operator=(const LoggingSession&) = delete;
    LoggingSession& operator=(LoggingSession&&) noexcept = delete;
    static LoggingSession& instance();
    void
    initialize(const char* name, Option logopt = (Option::CONS | Option::PERROR | Option::PID));
    LogLevel getLogLevel() const;
    void setLogLevel(LogLevel level);
    template <typename CharT, typename Traits, typename Allocator>
    void push(const std::basic_string<CharT, Traits, Allocator>& s)
    {
        m_semaphore.signal();
        m_stringbuffer.write(s);
    }
};

#endif // SYSTEM_LOGGER_LOGGING_SESSION_H
