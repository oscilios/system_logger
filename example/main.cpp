#include "system_logger/Logger.h"
#include <future>
#include <iostream>

using namespace system_logger;
using namespace std::chrono_literals;

static auto rnd_us()
{
    return std::chrono::microseconds(std::rand() % 1000 + 1);
}

int main()
{
    LoggingSession::instance().initialize("system_logger_example");
    LoggingSession::instance().setLogLevel(LogLevel::Debug);
    RtLogger logger("main");
    logger.setLogLevel(LogLevel::Warning);

    std::vector<std::thread> threads;
    std::atomic<bool> done{false};
    const auto lvalue = std::string("Hello world!");

    threads.emplace_back([&]() {
        while (!done)
        {
            SYSLOG_EMERGENCY(logger, lvalue, "================ this is Emergency", 0, rnd_us().count(), " ================");
            std::this_thread::sleep_for(rnd_us());
        }
    });
    threads.emplace_back([&]() {
        while (!done)
        {
            SYSLOG_ALERT(logger, lvalue, "================ this is Alert", 1, rnd_us().count(), " ================");
            std::this_thread::sleep_for(rnd_us());
        }
    });
    threads.emplace_back([&]() {
        while (!done)
        {
            SYSLOG_CRITICAL(logger, lvalue, "================ this is Critical", 2, rnd_us().count(), " ================");
            std::this_thread::sleep_for(rnd_us());
        }
    });
    threads.emplace_back([&]() {
        while (!done)
        {
            SYSLOG_ERROR(logger, lvalue, "================ this is Error", 3, rnd_us().count(), " ================");
            std::this_thread::sleep_for(rnd_us());
        }
    });
    threads.emplace_back([&]() {
        while (!done)
        {
            SYSLOG_WARNING(logger, lvalue, "================ this is Warning", 4, rnd_us().count(), " ================");
            std::this_thread::sleep_for(rnd_us());
        }
    });
    threads.emplace_back([&]() {
        while (!done)
        {
            SYSLOG_NOTICE(logger, lvalue, "================ this is Notice", 5, rnd_us().count(), " ================");
            std::this_thread::sleep_for(rnd_us());
        }
    });
    threads.emplace_back([&]() {
        while (!done)
        {
            SYSLOG_INFO(logger, lvalue, "================ this is Info", 6, rnd_us().count(), " ================");
            std::this_thread::sleep_for(rnd_us());
        }
    });
    threads.emplace_back([&]() {
        while (!done)
        {
            SYSLOG_DEBUG(logger, lvalue, "================ this is Debug", 7, rnd_us().count(), " ================");
            std::this_thread::sleep_for(rnd_us());
        }
    });

    std::this_thread::sleep_for(5s);
    done = true;
    for (auto& t : threads)
        t.join();

    SYSLOG_ERROR(logger, "========== DONE ===========");

    return 0;
}
