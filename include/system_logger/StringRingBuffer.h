#ifndef SYSTEM_LOGGER_THREADSAFE_RINGBUFFER_H
#define SYSTEM_LOGGER_THREADSAFE_RINGBUFFER_H

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
        template <typename CharT, int64_t TMaxSize, size_t MemorySizeBytes = 4096>
        class StringRingBuffer;
    }

    template <size_t N>
    struct IsPowerOfTwo
    {
        constexpr static bool value = (N > 0) && (N & (N - 1)) == 0;
    };

}

template <typename CharT, int64_t TMaxSize, size_t MemorySizeBytes>
class system_logger::threadsafe::StringRingBuffer final
{
    using StringAllocator = memory::Allocator<CharT, MemorySizeBytes>;
    using String          = std::basic_string<CharT, std::char_traits<CharT>, StringAllocator>;

    std::atomic<int64_t> m_readIdx{0};
    alignas(64) char m_padToAvoidFalseSharing1[64 - sizeof(size_t)];
    std::atomic<int64_t> m_writeIdx{0};
    std::atomic<int64_t> m_written{0};

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
        auto rIdx = m_readIdx.load(std::memory_order_relaxed);
        if (rIdx >= m_written.load(std::memory_order_acquire))
            return false;

        while (!m_readIdx.compare_exchange_weak(
            rIdx, rIdx + 1, std::memory_order_release, std::memory_order_relaxed))
            ;

        const auto& s = m_buffer[rIdx & m_bitmask];
        output.assign(s.data(), s.length());
        return true;
    }
    template <typename Alloc>
    bool write(const std::basic_string<CharT, std::char_traits<CharT>, Alloc>& input) noexcept
    {
        if (m_written.load(std::memory_order_relaxed) - m_readIdx.load(std::memory_order_acquire) >=
            TMaxSize - 1)
        {
            // buffer is full
            assert (false);
            return false;
        }

        auto wIdx = m_writeIdx.load(std::memory_order_relaxed);
        while (!m_writeIdx.compare_exchange_weak(
            wIdx, wIdx + 1, std::memory_order_release, std::memory_order_relaxed))
            ;

        m_buffer[wIdx & m_bitmask].assign(input.data(), input.length());
        m_written.fetch_add(1, std::memory_order_release);
        return true;
    }
};

#endif // SYSTEM_LOGGER_THREADSAFE_RINGBUFFER_H
