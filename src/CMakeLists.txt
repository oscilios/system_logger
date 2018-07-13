set (target system_logger)

set (headers
    ${CMAKE_SOURCE_DIR}/include/system_logger/Allocator.h
    ${CMAKE_SOURCE_DIR}/include/system_logger/BasicLogger.h
    ${CMAKE_SOURCE_DIR}/include/system_logger/LoggingSession.h
    ${CMAKE_SOURCE_DIR}/include/system_logger/Logger.h
    ${CMAKE_SOURCE_DIR}/include/system_logger/StringRingBuffer.h
    )

set (sources
    LoggingSession.cpp
    Semaphore.cpp
    )

add_library (${target} ${sources})

target_include_directories (${target}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

set_target_properties(${target}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    )

if(NOT BUILD_SHARED_LIBS)
    set_target_properties(${target} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

install (
    TARGETS ${target} EXPORT SystemLoggerTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/system_logger
    )
install (
    FILES ${headers}
    DESTINATION include/system_logger
    )

##### config package ####

include (CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/system_logger/system_logger-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

export(
    EXPORT SystemLoggerTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/system_logger/system_logger-targets.cmake"
    NAMESPACE system_logger::
    )

configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/system_logger-config.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/system_logger/system_logger-config.cmake"
    INSTALL_DESTINATION cmake
    )

set(ConfigPackageLocation lib/cmake/system_logger)

install(
    EXPORT SystemLoggerTargets
    FILE system_logger-targets.cmake
    NAMESPACE system_logger::
    DESTINATION ${ConfigPackageLocation}
    )

install(
    FILES
    ${CMAKE_SOURCE_DIR}/cmake/system_logger-config.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/system_logger/system_logger-version.cmake"
    DESTINATION ${ConfigPackageLocation}
    )
