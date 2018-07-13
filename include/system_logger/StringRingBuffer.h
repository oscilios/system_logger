#ifndef SYSTEM_LOGGER_THREADSAFE_RINGBUFFER_H
#define SYSTEM_LOGGER_THREADSAFE_RINGBUFFER_H

#include "system_logger/AtomicLock.h"
#include "system_logger/Allocator.h"

#include <atomic>
#include <cassert>
#include <string>
#include <type_traits>
#include <vector>

namespace system_logger
{
    namespace threadsafe
    {
        template <typename CharT, size_t TMaxSize, size_t MemorySizeBytes = 4096>
        class StringRingBuffer;
    }

    template <size_t N>
    struct IsPowerOfTwo
    {
        constexpr static bool value = (N > 0) && (N & (N - 1)) == 0;
    };

}

template <typename CharT, size_t TMaxSize, size_t MemorySizeBytes>
class system_logger::threadsafe::StringRingBuffer final
{
    using StringAllocator = memory::Allocator<CharT, MemorySizeBytes>;
    using String          = std::basic_string<CharT, std::char_traits<CharT>, StringAllocator>;

    std::atomic<bool> m_mutex{false};
    std::atomic<size_t> m_readIdx{0};
    std::atomic<size_t> m_writeIdx{0};

    typename StringAllocator::memory_pool_type m_memory;
    StringAllocator m_allocator;
    std::vector<String> m_buffer;
    static constexpr size_t m_bitmask{TMaxSize - 1};

public:
    StringRingBuffer() noexcept(false)
    : m_allocator(m_memory)
    , m_buffer(TMaxSize, String(m_allocator))
    {
        static_assert(IsPowerOfTwo<TMaxSize>::value,
                      "StringRingBuffer max size must be a power of two");
    }
    StringRingBuffer(const StringRingBuffer&)     = delete;
    StringRingBuffer(StringRingBuffer&&) noexcept = delete;
    StringRingBuffer& operator=(const StringRingBuffer&) = delete;
    StringRingBuffer& operator=(StringRingBuffer&&) noexcept = delete;
    ~StringRingBuffer()                                      = default;

    template <typename Alloc>
    bool read(std::basic_string<CharT, std::char_traits<CharT>, Alloc>& output) noexcept
    {
        AtomicLock lk(m_mutex);
        const size_t rIdx = m_readIdx.load(std::memory_order_relaxed);
        if (rIdx >= m_writeIdx.load(std::memory_order_relaxed))
            return false;

        const auto& s = m_buffer[rIdx & m_bitmask];
        output.assign(s.data(), s.length());
        m_readIdx.fetch_add(1, std::memory_order_release);
        return true;
    }
    template <typename Alloc>
    void write(const std::basic_string<CharT, std::char_traits<CharT>, Alloc>& input) noexcept
    {
        AtomicLock lk(m_mutex);
        const size_t wIdx = m_writeIdx.load(std::memory_order_relaxed);
        m_buffer[wIdx & m_bitmask].assign(input.data(), input.length());
        m_writeIdx.fetch_add(1, std::memory_order_release);
    }
};

#endif // SYSTEM_LOGGER_THREADSAFE_RINGBUFFER_H
