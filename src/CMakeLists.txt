include(CheckIncludeFile)
include(CheckFunctionExists)

check_include_file(malloc.h HAVE_MALLOC_H)
if(HAVE_MALLOC_H)
    add_definitions(-DHAVE_MALLOC_H=1)
endif()

check_include_file(alloca.h HAVE_ALLOCA_H)
if(HAVE_ALLOCA_H)
    add_definitions(-DHAVE_ALLOCA_H=1)
endif()

check_include_file(sys/time.h HAVE_SYS_TIME_H)
if(HAVE_SYS_TIME_H)
    add_definitions(-DHAVE_SYS_TIME_H=1)
endif()

check_function_exists(gettimeofday HAVA_GETTIMEOFDAY)
if(HAVA_GETTIMEOFDAY)
    add_definitions(-DHAVA_GETTIMEOFDAY=1)
endif()

check_include_file(pthread.h HAVE_PTHREAD_H)
if(NOT HAVE_PTHREAD_H)
    message(FATAL_ERROR
        "libcrystal depends on pthread, but pthread not found! "
        "slim-pthread or pthreads-win32 are recommended.")
endif()

set(SRC
    base58.c
    bitset.c
    ids_heap.c
    linkedhashtable.c
    linkedlist.c
    rc_mem.c
    vlog.c
    timerheap.c
    time_util.c
    socket.c
    spopen.c)

set(HEADERS
    base58.h
    bitset.h
    ids_heap.h
    linkedhashtable.h
    linkedlist.h
    rc_mem.h
    vlog.h
    timerheap.h
    time_util.h
    socket.h
    spopen.h
    crystal_config.h)

include_directories(.)
add_definitions(-DCRYSTAL_BUILD)

if(ENABLE_CRYPTO)
    set(SRC
        ${SRC}
        crypto.c)

    set(HEADERS
        ${HEADERS}
        crypto.h)

    if(NOT "${WITH_LIBSODIUM}" STREQUAL "")
        find_library(SODIUM
            NAMES sodium libsodium
            HINTS "${WITH_LIBSODIUM}/lib")

        include_directories("${WITH_LIBSODIUM}/include")
    else()
        find_library(SODIUM NAMES sodium libsodium)
    endif()

    message(STATUS "Found libsodium: ${SODIUM}")

    if(LIBSODIUM_STATIC)
        add_definitions(-DSODIUM_STATIC)
        message(STATUS "Link libsodium as static library")
    else()
        message(STATUS "Link libsodium as shared library")
    endif()
endif()

if(WIN32)
    set(SRC
        ${SRC}
        windows/gettimeofday.c
        windows/getopt_long.c)

    set(HEADERS
        ${HEADERS}
        windows/gettimeofday.h
        windows/getopt.h
        windows/posix_helper.h
        windows/builtins.h)

    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    include_directories(windows ${WITH_PTHREAD}/include)
    link_directories(${WITH_PTHREAD}/lib)
elseif(ANDROID)
    set(SRC
        ${SRC}
        android/ifaddrs.c)

    set(HEADERS
        ${HEADERS}
        android/ifaddrs.h)

    include_directories(android)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(SYSTEM_LIBS pthread m)
elseif(WIN32)
    set(SYSTEM_LIBS Ws2_32 Winmm pthread)
endif()

if(ANDROID)
    list(APPEND SYSTEM_LIBS log)
endif()

if(ENABLE_STATIC)
    add_library(crystal-static STATIC ${SRC})
    target_compile_definitions(crystal-static PRIVATE CRYSTAL_STATIC)
    set_target_properties(crystal-static PROPERTIES OUTPUT_NAME crystal)

    install(TARGETS crystal-static
        RUNTIME DESTINATION "bin"
        ARCHIVE DESTINATION "lib"
        LIBRARY DESTINATION "lib")
endif()

if(ENABLE_SHARED)
    if(APPLE)
        set(CMAKE_INSTALL_RPATH "@execuable_path/Frameworks;@loader_path/../lib")
    else()
        set(CMAKE_MACOSX_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    endif()
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR TRUE)
    set(CMAKE_INSTALL_NAME_DIR "@rpath")
    set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG "-Wl,-rpath,")
    set(CMAKE_SHARED_LIBRARY_RUNTIME_C_FLAG_SEP ":")

    add_library(crystal-shared SHARED ${SRC})
    target_compile_definitions(crystal-shared PRIVATE CRYSTAL_DYNAMIC)
    set_target_properties(crystal-shared PROPERTIES OUTPUT_NAME crystal)
    target_link_libraries(crystal-shared ${SODIUM} ${SYSTEM_LIBS})

    install(TARGETS crystal-shared
        RUNTIME DESTINATION "bin"
        ARCHIVE DESTINATION "lib"
        LIBRARY DESTINATION "lib")
endif()

install(FILES ${HEADERS} DESTINATION "include")
