project(tests C)

include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckSymbolExists)

check_include_file(sys/resource.h HAVE_SYS_RESOURCE_H)
if(HAVE_SYS_RESOURCE_H)
    add_definitions(-DHAVE_SYS_RESOURCE_H=1)
endif()

check_function_exists(setrlimit HAVE_SETRLIMIT)
if(HAVE_SETRLIMIT)
    add_definitions(-DHAVE_SETRLIMIT=1)
endif()

check_symbol_exists(SIGKILL "signal.h" HAVE_SIGKILL)
if(HAVE_SIGKILL)
    add_definitions(-DHAVE_SIGKILL=1)
endif()

check_symbol_exists(SIGHUP "signal.h" HAVE_SIGHUP)
if(HAVE_SIGHUP)
    add_definitions(-DHAVE_SIGHUP=1)
endif()

set(SRC
    tests.c
    bitset_test.c)

include_directories(
    .
    ../src
    ${WITH_LIBCUNIT}/include)

link_directories(${WITH_LIBCUNIT}/lib)

add_executable(unit_tests ${SRC})

if(WIN32)
    target_link_libraries(unit_tests
        crystal-shared
        libcunit)
else()
    target_link_libraries(unit_tests
        crystal-shared
        cunit)
endif()

install(TARGETS unit_tests
    RUNTIME DESTINATION "bin"
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib")
