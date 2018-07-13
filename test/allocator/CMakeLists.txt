set (target allocator_test)
set (sources
    main.cpp
    AllocatorTest.cpp
    )
add_executable (${target} ${sources})
add_test (NAME ${target} COMMAND ${target} --use-colour yes)
