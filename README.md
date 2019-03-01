# Libcrystal

## What is It

librarystal is a portable, easy-to-use as well as lightweight suite of auxilliary library with high performance used for front-end apps. It's goal is to provide the most useful and popular operations need to build higher-level applications and tools.

At the time of this writing, **libcrystal** is bundled with the following modules:
- Linkedlist
- Linkedhashtable
- Ids_heap
- Bitset
- Rc_mem
- Timerheap
- Spopen
- Vlog
- Base58
- Crypto
- Posix API adaptions on Windows.

## Table of Contents

- [Libcrystal](#libcrystal)
	- [What Is It](#what-is-it)
	- [Table of Contents](#table-of-contents)
- [Prerequisites](#prerequisites)
	- [CMake](#cmake)
	- [Sodium](#sodium)
	- [CUnit](#CUnit)
- [Build from Source](#build-from-source)
- [Run tests](#run-tests)
- [Contributions](#contributions)
- [License](#license)

# Prerequisites

## CMake

**CMake** is required to build and test this project in an operating system with compiler-independent manner.

At the time of this writing,  the compilation works on **MacOS**, **Linux** (Ubuntu, Debian etc) and even **Windows**.

## Sodium

[**Sodium**](https://github.com/jedisct1/libsodium)  is a set of library for encryption, decryption, signatures, password hashing and more.  With reference to **libsodium**, the compilation with **Crypto** module would be possible to proceed in the end with a build distribution containing **Crypto** feature.

## CUnit

[**CUnit**](https://sourceforge.net/projects/cunit) is well-known as an automated test framework for 'C'-based program. With that, unit-test suites for **libcrystal** APIs could be compiled and run on your device.

# Build from source

Assumed that libraries **libsodium** and **libcunit** have been compiled and distributed under the directory **YOUR-DEPS-PATH** with the following structure:

```
|--include
    |--CUnit
    |--sodium.h
    |--sodium
|--lib
    |--libcunit.a (or cunit.lib)
    |--libsodium.a (or sodium.lib)
```
Then use the following series of commands to build the project with full features on **Unix**-like operating system:
```shell
$ git clone https://github.com/iwhisperio/libcrystal.git
$ cd libcrystal
$ mkdir build && cd build
$ cmake -DENABLE_SHARED=ON \
        -DENABLE_STATIC=ON \
        -DENABLE_CRYPTO=ON \
        -DWITH_LIBSODIUM=YOUR-DEPS-PATH \
        -DLIBSODIUM_STATIC=ON \
        -DENABLE_TESTS=ON \
        -DWITH_LIBCUNIT=YOUR-DEPS-PATH \
        -DCMAKE_INSTALL_PREFIX=dist ..
$ make
$ make install
```

On **Windows**,  the extra **CMake** option **-G** should be used with the following commands:

```shell
$ cmake -G"NMake Makefiles" \
		-DENABLE_SHARED=ON \
        -DENABLE_STATIC=ON \
        -DENABLE_CRYPTO=ON \
        -DWITH_LIBSODIUM=YOUR-DEPS-PATH \
        -DLIBSODIUM_STATIC=ON \
        -DENABLE_TESTS=ON \
        -DWITH_LIBCUNIT=YOUR-DEPS-PATH \
        -DCMAKE_INSTALL_PREFIX=dist ..
$ nmake
$ nmake install
```


# Run tests

With compilation using **ENABLE_TESTS** option,  run the commands to verify the APIs on **MacOS** working or not:

```shell
$ cd dist/bin
$ DYLD_LIBRARY_PATH=../lib ./unit_tests
```

or on **Linux**:

```shell
$ cd dist/bin
$ LD_LIBRARY_PATH=../lib ./unit_tests
```

and on **Windows**, just run the commands:

```shell
$ cd dist/bin
$ unit_tests.exe
```

# Contribution

Welcome the contributions about ideas or new modules that could enrich this project.

# License

[MIT License](https://github.com/iwhisperio/libcrystal/blob/master/LICENSE)

