#include "system_logger/Semaphore.h"

using system_logger::threadsafe::Semaphore;

#ifdef __APPLE__
#include <dispatch/dispatch.h>
class Semaphore::Impl
{
    dispatch_semaphore_t m_sem;

public:
    explicit Impl(int initialValue)
    : m_sem(dispatch_semaphore_create(initialValue))
    {
    }
    Impl(const Impl&) = delete;
    Impl(Impl&&)      = delete;
    ~Impl()
    {
        dispatch_release(m_sem);
    }
    Impl& operator=(const Impl&) = delete;
    Impl& operator=(Impl&&) = delete;
    int64_t wait()
    {
        return dispatch_semaphore_wait(m_sem, DISPATCH_TIME_FOREVER);
    }
    void signal()
    {
        dispatch_semaphore_signal(m_sem);
    }
};
#else
#error "Semaphore not defined for other platforms than OSX."
#endif

Semaphore::Semaphore(int initialValue)
: m_sem(std::make_unique<Semaphore::Impl>(initialValue))
{
}
Semaphore::~Semaphore() = default;
int64_t Semaphore::wait()
{
    return m_sem->wait();
}
void Semaphore::signal()
{
    m_sem->signal();
}
