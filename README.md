# Libcrystal

## What is It

The **librarystal** is a portable, easy-to-use and lightweight suite of auxliary library with high performance used for front-end apps. It's goal is to provide the most useful and popular APIs that could likely help for building higher-level applications and tools.

At the time of writing, **libcrystal** is bundled with the following modules:

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
	- [What is It](#what-is-it)
	- [Table of Contents](#table-of-contents)
- [Prerequisites](#prerequisites)
	- [CMake](#1-cmake)
	- [Sodium](#2-sodium)
	- [CUnit](#3-cunit)
	- [Pthread](#4-pthread)
- [Build from Source](#build-from-source)
- [Run tests](#run-tests)
- [Contributions](#contributions)
- [License](#license)

# Prerequisites

## 1. CMake

**CMake** is required to build and test this project in an operating system with compiler-independent manner.

At the time of this writing,  the compilation works on **MacOS**, **Linux** (Ubuntu, Debian etc) and even **Windows**.

## 2. Sodium

[**Sodium**](https://github.com/jedisct1/libsodium) is a set of library for encryption, decryption, signatures, password hashing and more. With reference to **libsodium**, the compilation with **Crypto** module would be possible to proceed in the end with a build distribution containing **Crypto** feature.

## 3. CUnit

[**CUnit**](https://sourceforge.net/projects/cunit) is well-known as an automated test framework for 'C'-based program. With that, unit-test suites for **libcrystal** APIs could be compiled and run on your device.

## 4. Pthread on Windows

**Pthread** is a set of pthread APIs with POSIX 1003.1c standard for Windows platform. You have to choose to use regular [**pthread-win32**](https://github.com/GerHobbelt/pthread-win32) or lightweight [**slim-pthread**](https://github.com/iwhisperio/slim-pthread) when building **libcrystal** on Windows platform.

# Build from source

Assumed that libraries **libsodium** and **libcunit** have been compiled and distributed under the directory **YOUR-DEPS-PATH** with following structures:
```markdown
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

On **Windows**, besides option **-DWITH_PTHREAD**,  the extra **CMake** option **-G** should be used with following commands:

```shell
$ cmake -G"NMake Makefiles" \
		-DENABLE_SHARED=ON \
        -DENABLE_STATIC=ON \
        -DENABLE_CRYPTO=ON \
        -DWITH_LIBSODIUM=YOUR-DEPS-PATH \
        -DLIBSODIUM_STATIC=ON \
        -DENABLE_TESTS=ON \
        -DWITH_LIBCUNIT=YOUR-DEPS-PATH \
        -DWITH_PTHREAD=YOUR-DEPS-PATH \
        -DCMAKE_INSTALL_PREFIX=dist ..
$ nmake
$ nmake install
```

With Shorthand, run the command below:
```shell
$ DEPSDIR=YOUR-DEPS-PATH ./compile.sh
```

# Run tests

With compilation using **ENABLE_TESTS** option,  run the commands to verify the APIs on **MacOS** working or not::

```shell
$ cd dist/bin
$ DYLD_LIBRARY_PATH=../lib ./unit_tests
```

Or on **Linux**:

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

