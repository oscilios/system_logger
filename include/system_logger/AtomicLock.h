#ifndef SYSTEM_LOGGER_ATOMIC_LOCK_H
#define SYSTEM_LOGGER_ATOMIC_LOCK_H

#include <atomic>

namespace system_logger
{
    namespace threadsafe
    {
        class AtomicLock;
    }
}
class system_logger::threadsafe::AtomicLock
{
    std::atomic<bool>& m_mtx;

public:
    explicit AtomicLock(std::atomic<bool>& mtx)
    : m_mtx(mtx)
    {
        while (m_mtx.exchange(true))
        {
        }
    }
    AtomicLock(const AtomicLock&) = delete;
    AtomicLock(AtomicLock&&)      = delete;
    ~AtomicLock()
    {
        m_mtx = false;
    }
    AtomicLock& operator=(const AtomicLock&) = delete;
    AtomicLock& operator=(AtomicLock&&) = delete;
};
#endif // SYSTEM_LOGGER_ATOMIC_LOCK_H
