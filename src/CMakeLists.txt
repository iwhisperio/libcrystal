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
    BR/BRBase58.c
    BR/BRCrypto.c
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
    ../include/crystal/crystal_config.h
    ../include/crystal/bitset.h
    ../include/crystal/ids_heap.h
    ../include/crystal/linkedhashtable.h
    ../include/crystal/linkedlist.h
    ../include/crystal/rc_mem.h
    ../include/crystal/socket.h
    ../include/crystal/spopen.h
    ../include/crystal/time_util.h
    ../include/crystal/timerheap.h
    ../include/crystal/vlog.h)

include_directories(BEFORE ../include BR)

if(ENABLE_BASE58)
    set(SRC
        ${SRC}
        base58.c)
    set(HEADERS
        ${HEADERS}
        ../include/crystal/base58.h)

    set(INCLUDE_BASE58_H_STRING [=[#include <crystal/base58.h>]=])
endif()

add_definitions(-DCRYSTAL_BUILD)

if(ENABLE_CRYPTO)
    set(SRC
        ${SRC}
        crypto.c)

    set(HEADERS
        ${HEADERS}
        ../include/crystal/crypto.h)

    set(INCLUDE_CRYPTO_H_STRING [=[#include <crystal/crypto.h>]=])

    if(NOT "${WITH_LIBSODIUM}" STREQUAL "")
        find_library(SODIUM
            NAMES sodium libsodium
            HINTS "${WITH_LIBSODIUM}/lib"
            NO_DEFAULT_PATH)
        find_file(SODIUM_HEADER sodium.h
            HINTS "${WITH_LIBSODIUM}/include"
            NO_DEFAULT_PATH)
    else()
        find_library(SODIUM NAMES sodium libsodium)
        find_file(SODIUM_HEADER sodium.h)
    endif()

    if("${SODIUM}" STREQUAL SODIUM-NOTFOUND)
        message(FATAL_ERROR "libsodium library not found")
    endif()
    if("${SODIUM_HEADER}" STREQUAL SODIUM_HEADER-NOTFOUND)
        message(FATAL_ERROR "libsodium header not found")
    endif()

    message(STATUS "Found libsodium: ${SODIUM}")
    get_filename_component(SODIUM_INCLUDE_DIR "${SODIUM_HEADER}" DIRECTORY CACHE)
    include_directories(BEFORE "${SODIUM_INCLUDE_DIR}")

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
        windows/getopt_long.c
        windows/strptime.c)

    set(HEADERS
        ${HEADERS}
        ../include/crystal/gettimeofday.h
        ../include/crystal/getopt.h
        ../include/crystal/posix_helper.h
        ../include/crystal/builtins.h
        ../include/crystal/strptime.h)

    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)

    include_directories(BEFORE "${WITH_PTHREAD}/include")
    link_directories("${WITH_PTHREAD}/lib")
elseif(ANDROID)
    set(SRC
        ${SRC}
        android/ifaddrs.c)

    set(HEADERS
        ${HEADERS}
        ../include/crystal/ifaddrs.h)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(SYSTEM_LIBS pthread m)

    ## if raspiberryPi
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm")
        list(APPEND SYSTEM_LIBS rt)
    endif()
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

install(FILES ${HEADERS} DESTINATION "include/crystal")

set(CRYSTAL_H "${CMAKE_BINARY_DIR}/include/crystal.h")
configure_file(
    ../include/crystal.h.in
    "${CRYSTAL_H}"
    @ONLY)
install(FILES "${CRYSTAL_H}" DESTINATION "include")
