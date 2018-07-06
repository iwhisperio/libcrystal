/**
 * This file is dual licensed under the MIT and the University of Illinois Open
 * Source Licenses.
 *
 * Origin: llvm - libcxx: include/support/win32/support.h
 */

#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#ifdef _MSC_VER // For Microsoft Visual C/C++ only

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

// Bit builtin's make these assumptions when calling _BitScanForward/Reverse etc.
// These assumptions are expected to be true for Win32/Win64 which this file supports.
static_assert(sizeof(unsigned long long)==8,"");
static_assert(sizeof(unsigned long)==4,"");
static_assert(sizeof(unsigned int)==4,"");

static __inline
int __builtin_popcount(unsigned int x)
{
    static const unsigned int m1 = 0x55555555; //binary: 0101...
    static const unsigned int m2 = 0x33333333; //binary: 00110011..
    static const unsigned int m4 = 0x0f0f0f0f; //binary:  4 zeros,  4 ones ...
    static const unsigned int h01= 0x01010101; //the sum of 256 to the power of 0,1,2,3...
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
    return (x * h01) >> 24;  //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24)
}

static __inline
int __builtin_popcountl(unsigned long x)
{
    return __builtin_popcount((int)(x));
}

static __inline
int __builtin_popcountll(unsigned long long x)
{
    static const unsigned long long m1  = 0x5555555555555555; //binary: 0101...
    static const unsigned long long m2  = 0x3333333333333333; //binary: 00110011..
    static const unsigned long long m4  = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
    static const unsigned long long h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
    return (int)((x * h01)>>56);  //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
}

static __inline
int __builtin_ctzll(unsigned long long mask)
{
    unsigned long where;
    // Search from LSB to MSB for first set bit.
    // Returns zero if no set bit is found.
#if defined(_WIN64)
    if (BitScanForward64(&where, mask))
        return (int)(where);
#elif defined(_WIN32)
    // Win32 doesn't have _BitScanForward64 so emulate it with two 32 bit calls.
    // Scan the Low Word
    if (BitScanForward(&where, (unsigned long)(mask)))
        return (int)(where);
    // Scan the High Word
    if (BitScanForward(&where, (unsigned long)(mask >> 32)))
        return (int)(where + 32); // Create a bit offset from the LSB.
#endif
    return 64;
}

static __inline
int __builtin_ctzl(unsigned long mask)
{
    unsigned long where;
    // Search from LSB to MSB for first set bit.
    // Returns zero if no set bit is found.
    if (BitScanForward(&where, mask))
        return (int)(where);
    return 32;
}

static __inline
int __builtin_ctz(unsigned int mask)
{
    // Win32 and Win64 expectations.
    static_assert(sizeof(mask)==4,"");
    static_assert(sizeof(unsigned long)==4,"");
    return __builtin_ctzl((unsigned long)(mask));
}

static __inline
int __builtin_clzll(unsigned long long mask)
{
    unsigned long where;
    // BitScanReverse scans from MSB to LSB for first set bit.
    // Returns 0 if no set bit is found.
#if defined(_WIN64)
    if (BitScanReverse64(&where, mask))
        return (int)(63 - where);
#elif defined(_WIN32)
    // Scan the high 32 bits.
    if (BitScanReverse(&where, (unsigned long)(mask >> 32)))
        return (int)(63 - (where + 32)); // Create a bit offset from the MSB.
    // Scan the low 32 bits.
    if (BitScanReverse(&where, (unsigned long)(mask)))
        return (int)(63 - where);
#endif
    return 64; // Undefined Behavior.
}

static __inline
int __builtin_clzl(unsigned long mask)
{
    unsigned long where;
    // Search from LSB to MSB for first set bit.
    // Returns zero if no set bit is found.
    if (BitScanReverse(&where, mask))
        return (int)(31-where);
    return 32; // Undefined Behavior.
}

static __inline
int __builtin_clz(unsigned int x)
{
    return __builtin_clzl(x);
}

static __inline
unsigned int __sync_sub_and_fetch(unsigned int *ptr, unsigned int value)
{
    long _value = -(long)value;
    unsigned int rc = (unsigned int)InterlockedExchangeAdd((LONG *)ptr, (LONG)_value);
    return rc + _value;
}

static __inline
unsigned int __sync_add_and_fetch(unsigned int *ptr, unsigned int value)
{
    unsigned int rc = (unsigned int)InterlockedExchangeAdd((LONG *)ptr, (LONG)value);
    return rc + value;
}

#ifdef __cplusplus
}
#endif

#endif /* _MSC_VER */

#endif /* __BUILTINS_H__ */
