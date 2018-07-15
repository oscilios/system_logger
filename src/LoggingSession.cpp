#include "system_logger/LoggingSession.h"

#include <cmath> // log
#include <sstream>
#include <syslog.h>

using system_logger::LoggingSession;
using system_logger::Option;
using system_logger::LogLevel;

Option system_logger::operator|(Option lhs, Option rhs)
{
    return static_cast<Option>(static_cast<std::underlying_type<Option>::type>(lhs) |
                               static_cast<std::underlying_type<Option>::type>(rhs));
}

namespace
{
    int computeLogLevelFromMask(int mask)
    {
        return int(std::log(mask + 1) / log(2)) - 1;
    }

    int computeMaxLogLevel(int loglevel)
    {
        const int mask = setlogmask(LOG_UPTO(LOG_EMERG));
        const int maxMask = (2 << loglevel) - 1;
        return computeLogLevelFromMask(mask | maxMask);
    }
    int translate(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::Emergency:
                return LOG_EMERG;
            case LogLevel::Alert:
                return LOG_ALERT;
            case LogLevel::Critical:
                return LOG_CRIT;
            case LogLevel::Error:
                return LOG_ERR;
            case LogLevel::Warning:
                return LOG_WARNING;
            case LogLevel::Notice:
                return LOG_NOTICE;
            case LogLevel::Info:
                return LOG_INFO;
            case LogLevel::Debug:
                return LOG_DEBUG;
            default:
                return LOG_DEBUG;
        }
    }
}

#ifndef NDEBUG
LogLevel LoggingSession::m_loglevel = LogLevel::Debug;
int LoggingSession::m_syslogLevel = LOG_DEBUG;
#else
LogLevel LoggingSession::m_loglevel = LogLevel::Warning;
int LoggingSession::m_syslogLevel = LOG_WARNING;
#endif

LoggingSession& LoggingSession::instance()
{
    static LoggingSession session;
    return session;
}
void LoggingSession::initialize(const char* name, Option logopt)
{
    std::call_once(m_initFlag, [this, &name, logopt]() {
        m_syslogLevel = computeMaxLogLevel(translate(m_loglevel));
        openlog(name, static_cast<std::underlying_type<Option>::type>(logopt), LOG_DAEMON);
        setlogmask(LOG_UPTO(m_syslogLevel));
        m_thread = std::thread(&LoggingSession::pop, this);
    });
}
LoggingSession::~LoggingSession()
{
    m_done = true;
    m_semaphore.signal();
    if (m_thread.joinable())
        m_thread.join();
    closelog();
}
system_logger::LogLevel LoggingSession::getLogLevel() const
{
    return m_loglevel;
}
void LoggingSession::setLogLevel(LogLevel level)
{
    m_loglevel = level;
    m_syslogLevel = translate(level);
    setlogmask(LOG_UPTO(m_syslogLevel));
}
void LoggingSession::pop()
{
    using StringAllocator = memory::Allocator<char_type, 1024>;
    using String          = std::basic_string<char_type, std::char_traits<char_type>, StringAllocator>;
    StringAllocator::memory_pool_type memory;
    StringAllocator allocator{memory};
    String s{allocator};
    while (!m_done)
    {
        m_semaphore.wait();
        while (m_stringbuffer.read(s))
        {
            // first char encodes the log level;
            syslog(translate(LogLevel(s.front())), s.data() + 1, "%s"); // NOLINT
        }
    }
}

