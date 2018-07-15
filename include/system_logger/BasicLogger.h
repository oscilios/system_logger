#ifndef SYSTEM_LOGGER_BASIC_LOGGER_H
#define SYSTEM_LOGGER_BASIC_LOGGER_H

#include "system_logger/AtomicLock.h"
#include "system_logger/LoggingSession.h"
#include <sstream>

namespace system_logger
{
    template <class CharT        = char_type,
              class Traits       = std::char_traits<CharT>,
              typename Allocator = std::allocator<CharT>>
    class BasicLogger;

    template <typename T>
    struct ApendSpace;
}

template <typename T>
struct system_logger::ApendSpace
{
    const T& x;
    ApendSpace(const T& value)
    : x(value)
    {
    }
    friend std::ostream& operator<<(std::ostream& out, ApendSpace value)
    {
        return out << value.x << ' ';
    }
};

template <class CharT, class Traits, typename AllocatorT>
class system_logger::BasicLogger final
{
    const std::string m_name;
    std::basic_ostringstream<CharT, Traits, AllocatorT> ss;
    std::atomic<bool> m_mutex;
    LogLevel m_loglevel{LoggingSession::instance().getLogLevel()};

public:
    BasicLogger(std::string name)
    : m_name(std::move(name))
    {
    }
    BasicLogger(const BasicLogger&) = delete;
    BasicLogger(BasicLogger&&)      = delete;
    ~BasicLogger()                  = default;
    BasicLogger& operator=(const BasicLogger&) = delete;
    BasicLogger& operator=(BasicLogger&&) = default;
    void setLogLevel(LogLevel level)
    {
        m_loglevel = level;
    }
    LogLevel getLogLevel() const
    {
        return m_loglevel;
    }
    template <typename... Ts>
    void operator()(LogLevel level, Ts&&... args)
    {
        static_assert(sizeof...(args) > 0, "No values to log");

        auto& session = LoggingSession::instance();
        if (level > m_loglevel || level > session.getLogLevel())
            return;

        threadsafe::AtomicLock lk(m_mutex);
        ss << static_cast<char>(level) << "[" << std::this_thread::get_id() << "] " << m_name
           << ": ";
        (ss << ... << ApendSpace<Ts>(args));
        session.push(ss.str());
        ss.str("");
    }
};
#endif // SYSTEM_LOGGER_BASIC_LOGGER_H
