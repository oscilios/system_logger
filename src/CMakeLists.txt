set (target system_logger)
set (headers
    ${CMAKE_SOURCE_DIR}/include/system_logger/LoggingSession.h
    ${CMAKE_SOURCE_DIR}/include/system_logger/Logger.h
    ${CMAKE_SOURCE_DIR}/include/system_logger/StringRingBuffer.h
    ${CMAKE_SOURCE_DIR}/include/system_logger/Allocator.h
    )
set (sources
    LoggingSession.cpp
    Semaphore.cpp
    )
add_library (${target} ${sources})
set_target_properties(
    ${target} PROPERTIES
    VERSION ${PROJECT_VERSION}
    )
