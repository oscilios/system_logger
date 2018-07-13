#include "system_logger/Allocator.h"
#include "../catch.hpp"
#include <new>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <limits>
#include <memory>

// Replace new and delete just for the purpose of demonstrating that
//  they are not called.

std::size_t memory = 0;
std::size_t alloc = 0;

void* operator new(std::size_t s) noexcept(false)
{
    memory += s;
    ++alloc;
    return malloc(s); // NOLINT
}

void  operator delete(void* p) throw()
{
    --alloc;
    free(p); // NOLINT
}

using namespace system_logger::memory;

template <class T, std::size_t Bytes = 256>
using VectorAllocator = Allocator<T, Bytes, alignof(T)>;

template <class T, std::size_t Bytes = 256>
using Vector = std::vector<T, VectorAllocator<T,Bytes>>;

using StringAllocator = Allocator<char, 64*64, alignof(char)>;
using String = std::basic_string<char, std::char_traits<char>, StringAllocator>;

using StringAllocatorWithInternalMemory = AllocatorWithInternalMemory<char, 64*64, alignof(char)>;
using OStringStream = std::basic_ostringstream<char, std::char_traits<char>, StringAllocatorWithInternalMemory>;


TEST_CASE("no memory is allocated in vector", "[AllocatorTest]")
{
    VectorAllocator<int>::memory_pool_type memoryPool;
    VectorAllocator<int> allocator(memoryPool);
    Vector<int> v{allocator};
    alloc = 0;
    memory = 0;

    for (int i=0; i<10; i++)
    {
        v.push_back(i);
        REQUIRE(v[i] == i);
    }

    REQUIRE(alloc == 0);
    REQUIRE(memory == 0);

}

TEST_CASE("no memory is allocated in string", "[AllocatorTest]")
{
    StringAllocator::memory_pool_type memoryPool;
    StringAllocator allocator(memoryPool);
    alloc = 0;
    memory =0;

    String s(allocator);
    s = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    REQUIRE(alloc == 0);
    REQUIRE(memory == 0);
    REQUIRE(s == "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    s += " bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
    s += " cccccccccccccccccccccccccccccccccccccccccccccccccc";
    s += " dddddddddddddddddddddddddddddddddddddddddddddddddd";
    s += " eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee";
    s += " ffffffffffffffffffffffffffffffffffffffffffffffffff";
    s += " gggggggggggggggggggggggggggggggggggggggggggggggggg";
    s += " hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh";
    s += " iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii";
    REQUIRE(alloc == 0);
    REQUIRE(memory == 0);
}
TEST_CASE("no memory is allocated in stream", "[AllocatorTest]")
{
    OStringStream ss;
    alloc = 0;
    memory =0;

    ss << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    REQUIRE(alloc == 0);
    REQUIRE(memory == 0);
    REQUIRE(ss.str() == "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

    ss << " bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb "
       << " cccccccccccccccccccccccccccccccccccccccccccccccccc "
       << 1234567890
       << " dddddddddddddddddddddddddddddddddddddddddddddddddd "
       << std::numeric_limits<int>::min()
       << " eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee "
       << std::numeric_limits<float>::max()
       << " ffffffffffffffffffffffffffffffffffffffffffffffffff "
       << std::numeric_limits<double>::max()
       << " gggggggggggggggggggggggggggggggggggggggggggggggggg "
       << std::numeric_limits<size_t>::max()
       << " hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh "
       << " iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii "
       << M_PI;

    REQUIRE(alloc == 0);
    REQUIRE(memory == 0);
    REQUIRE (ss.str().length() > 1000);

    StringAllocator::memory_pool_type memoryPool;
    StringAllocator allocator(memoryPool);
    String s(ss.str().data(), ss.str().length(), allocator);

    REQUIRE(alloc == 0);
    REQUIRE(memory == 0);

    std::string s2(ss.str().data(), ss.str().length());

    REQUIRE(alloc > 0);
    REQUIRE(memory > 0);
}
TEST_CASE("allocators compare equal", "[AllocatorTest]")
{
    StringAllocator::memory_pool_type memoryPool;
    StringAllocator allocator(memoryPool);
    String s1(allocator);
    String s2(allocator);
    REQUIRE(s1 == s2);
    REQUIRE(s1.get_allocator() == s2.get_allocator());
}
