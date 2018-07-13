set (target allocator_test)
set (sources
    main.cpp
    AllocatorTest.cpp
    )
add_executable (${target} ${sources})
target_link_libraries(${target} system_logger)
add_test (NAME ${target} COMMAND ${target} --use-colour yes)
