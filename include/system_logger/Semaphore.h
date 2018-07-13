#ifndef SYSTEM_LOGGER_SEMAPHORE_H
#define SYSTEM_LOGGER_SEMAPHORE_H

#include <memory>
#include <cstddef>

namespace system_logger
{
    namespace threadsafe
    {
        class Semaphore;
    }
}
class system_logger::threadsafe::Semaphore
{
    class Impl;
    std::unique_ptr<Impl> m_sem;

public:
    explicit Semaphore(int initialValue = 0);
    Semaphore(const Semaphore&) = delete;
    Semaphore(Semaphore&&)      = delete;
    ~Semaphore();
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;
    int64_t wait();
    void signal();
};
#endif // SYSTEM_LOGGER_SEMAPHORE_H
