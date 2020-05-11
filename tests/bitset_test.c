#include <stdlib.h>
#include <CUnit/Basic.h>

#include "crystal.h"

static void bitset_init_test(void)
{
    int i;

    BITSET(bitset1, 10);
    bitset_init((bitset_t *)&bitset1, 10);

    CU_ASSERT_EQUAL(bitset1.size, 10);
    CU_ASSERT_EQUAL(sizeof(bitset1.bits), sizeof(uint64_t))

    for (i = 0; i < sizeof(bitset1.bits) / sizeof(uint64_t); i++)
        CU_ASSERT_EQUAL(bitset1.bits[i], 0);

    BITSET(bitset2, 64);
    bitset_init((bitset_t *)&bitset2, 64);

    CU_ASSERT_EQUAL(bitset2.size, 64);
    CU_ASSERT_EQUAL(sizeof(bitset2.bits), sizeof(uint64_t))

    for (i = 0; i < sizeof(bitset2.bits) / sizeof(uint64_t); i++)
        CU_ASSERT_EQUAL(bitset2.bits[i], 0);

    BITSET(bitset3, 65);

    bitset_init((bitset_t *)&bitset3, 65);

    CU_ASSERT_EQUAL(bitset3.size, 65);
    CU_ASSERT_EQUAL(sizeof(bitset3.bits), sizeof(uint64_t) * 2)

    for (i = 0; i < sizeof(bitset3.bits) / sizeof(uint64_t); i++)
        CU_ASSERT_EQUAL(bitset3.bits[i], 0);

    BITSET(bitset4, 1025);

    bitset_init((bitset_t *)&bitset4, 1025);

    CU_ASSERT_EQUAL(bitset4.size, 1025);
    CU_ASSERT_EQUAL(sizeof(bitset4.bits), sizeof(uint64_t) * 17)

    for (i = 0; i < sizeof(bitset4.bits) / sizeof(uint64_t); i++)
        CU_ASSERT_EQUAL(bitset4.bits[i], 0);
}

static void bitset_size_test(void)
{
    BITSET(bitset1, 11);
    bitset_init((bitset_t *)&bitset1, 11);

    CU_ASSERT_EQUAL(bitset_size((bitset_t *)&bitset1), 11);

    BITSET(bitset2, 1);
    bitset_init((bitset_t *)&bitset2, 1);

    CU_ASSERT_EQUAL(bitset_size((bitset_t *)&bitset2), 1);

    BITSET(bitset3, 65);
    bitset_init((bitset_t *)&bitset3, 65);

    CU_ASSERT_EQUAL(bitset_size((bitset_t *)&bitset3), 65);

    BITSET(bitset4, 2047);
    bitset_init((bitset_t *)&bitset4, 2047);

    CU_ASSERT_EQUAL(bitset_size((bitset_t *)&bitset4), 2047);
}

static void bitset_set_and_clear_test(void)
{
    BITSET(bitset1, 1025);
    int size = 1025;
    int i;

    bitset_init((bitset_t *)&bitset1, 1025);

    for (i = 0; i < size; i++)
        CU_ASSERT_EQUAL(bitset_isset((bitset_t *)&bitset1, i), 0);

    for (i = 0; i < 1025 / 64; i++)
        CU_ASSERT_EQUAL(bitset1.bits[i], 0);
    CU_ASSERT_EQUAL(bitset1.bits[i], 0);

    for (i = 0; i < size; i++)
        bitset_set((bitset_t *)&bitset1, i);

    for (i = 0; i < size; i++)
        CU_ASSERT_EQUAL(bitset_isset((bitset_t *)&bitset1, i), 1);

    for (i = 0; i < 1025 / 64; i++)
        CU_ASSERT_EQUAL(bitset1.bits[i], 0xFFFFFFFFFFFFFFFF);
    CU_ASSERT_EQUAL(bitset1.bits[i], 1);

    for (i = 0; i < size; i++)
        bitset_clear((bitset_t *)&bitset1, i);

    for (i = 0; i < size; i++)
        CU_ASSERT_EQUAL(bitset_isset((bitset_t *)&bitset1, i), 0);

    for (i = 0; i < 1025 / 64; i++)
        CU_ASSERT_EQUAL(bitset1.bits[i], 0);
    CU_ASSERT_EQUAL(bitset1.bits[i], 0);

    for (i = 0; i < 1025; i += 64)
        bitset_set((bitset_t *)&bitset1, i);

    for (i = 0; i < 1025 / 64; i++)
        CU_ASSERT_EQUAL(bitset1.bits[i], 1);
    CU_ASSERT_EQUAL(bitset1.bits[i], 1);
}

static void bitset_prev_set_bit_test(void)
{
    BITSET(bitset1, 1001);
    int i;

    bitset_init((bitset_t *)&bitset1, 1001);

    CU_ASSERT_EQUAL(bitset_prev_set_bit((bitset_t *)&bitset1, 1000), -1);
    CU_ASSERT_EQUAL(bitset_prev_set_bit((bitset_t *)&bitset1, 499), -1);
    CU_ASSERT_EQUAL(bitset_prev_set_bit((bitset_t *)&bitset1, 1), -1);

    for (i = 0; i < 500; i++)
        bitset_set((bitset_t *)&bitset1, i);

    CU_ASSERT_EQUAL(bitset_prev_set_bit((bitset_t *)&bitset1, 1000), 499);
    CU_ASSERT_EQUAL(bitset_prev_set_bit((bitset_t *)&bitset1, 500), 499);
    CU_ASSERT_EQUAL(bitset_prev_set_bit((bitset_t *)&bitset1, 499), 499);

    bitset_clear((bitset_t *)&bitset1, 499);

    CU_ASSERT_EQUAL(bitset_prev_set_bit((bitset_t *)&bitset1, 1000), 498);
    CU_ASSERT_EQUAL(bitset_prev_set_bit((bitset_t *)&bitset1, 500), 498);
    CU_ASSERT_EQUAL(bitset_prev_set_bit((bitset_t *)&bitset1, 499), 498);

    bitset_set((bitset_t *)&bitset1, 510);

    CU_ASSERT_EQUAL(bitset_prev_set_bit((bitset_t *)&bitset1, 1000), 510);
    CU_ASSERT_EQUAL(bitset_prev_set_bit((bitset_t *)&bitset1, 500), 498);
    CU_ASSERT_EQUAL(bitset_prev_set_bit((bitset_t *)&bitset1, 499), 498);
}

static void bitset_next_set_bit_test(void)
{
    BITSET(bitset1, 1001);
    int i;

    bitset_init((bitset_t *)&bitset1, 1001);

    CU_ASSERT_EQUAL(bitset_next_set_bit((bitset_t *)&bitset1, 1000), -1);
    CU_ASSERT_EQUAL(bitset_next_set_bit((bitset_t *)&bitset1, 499), -1);
    CU_ASSERT_EQUAL(bitset_next_set_bit((bitset_t *)&bitset1, 0), -1);

    for (i = 500; i < 1001; i++)
        bitset_set((bitset_t *)&bitset1, i);

    CU_ASSERT_EQUAL(bitset_next_set_bit((bitset_t *)&bitset1, 1), 500);
    CU_ASSERT_EQUAL(bitset_next_set_bit((bitset_t *)&bitset1, 500), 500);
    CU_ASSERT_EQUAL(bitset_next_set_bit((bitset_t *)&bitset1, 501), 501);

    bitset_clear((bitset_t *)&bitset1, 500);

    CU_ASSERT_EQUAL(bitset_next_set_bit((bitset_t *)&bitset1, 1), 501);
    CU_ASSERT_EQUAL(bitset_next_set_bit((bitset_t *)&bitset1, 500), 501);
    CU_ASSERT_EQUAL(bitset_next_set_bit((bitset_t *)&bitset1, 501), 501);

    bitset_set((bitset_t *)&bitset1, 10);

    CU_ASSERT_EQUAL(bitset_next_set_bit((bitset_t *)&bitset1, 1), 10);
    CU_ASSERT_EQUAL(bitset_next_set_bit((bitset_t *)&bitset1, 500), 501);
    CU_ASSERT_EQUAL(bitset_next_set_bit((bitset_t *)&bitset1, 501), 501);
}

static void bitset_prev_clear_bit_test(void)
{
    BITSET(bitset1, 1001);
    int i;

    bitset_init((bitset_t *)&bitset1, 1001);

    for (i = 0; i < 500; i++)
        bitset_set((bitset_t *)&bitset1, i);

    CU_ASSERT_EQUAL(bitset_prev_clear_bit((bitset_t *)&bitset1, 1000), 1000);
    CU_ASSERT_EQUAL(bitset_prev_clear_bit((bitset_t *)&bitset1, 500), 500);
    CU_ASSERT_EQUAL(bitset_prev_clear_bit((bitset_t *)&bitset1, 499), -1);

    bitset_clear((bitset_t *)&bitset1, 499);

    CU_ASSERT_EQUAL(bitset_prev_clear_bit((bitset_t *)&bitset1, 1000), 1000);
    CU_ASSERT_EQUAL(bitset_prev_clear_bit((bitset_t *)&bitset1, 500), 500);
    CU_ASSERT_EQUAL(bitset_prev_clear_bit((bitset_t *)&bitset1, 499), 499);

    bitset_clear((bitset_t *)&bitset1, 10);

    CU_ASSERT_EQUAL(bitset_prev_clear_bit((bitset_t *)&bitset1, 1000), 1000);
    CU_ASSERT_EQUAL(bitset_prev_clear_bit((bitset_t *)&bitset1, 500), 500);
    CU_ASSERT_EQUAL(bitset_prev_clear_bit((bitset_t *)&bitset1, 499), 499);
    CU_ASSERT_EQUAL(bitset_prev_clear_bit((bitset_t *)&bitset1, 498), 10);
    CU_ASSERT_EQUAL(bitset_prev_clear_bit((bitset_t *)&bitset1, 10), 10);
    CU_ASSERT_EQUAL(bitset_prev_clear_bit((bitset_t *)&bitset1, 9), -1);
}

static void bitset_next_clear_bit_test(void)
{
    BITSET(bitset1, 1001);
    int i;

    bitset_init((bitset_t *)&bitset1, 1001);

    for (i = 500; i < 10001; i++)
        bitset_set((bitset_t *)&bitset1, i);

    CU_ASSERT_EQUAL(bitset_next_clear_bit((bitset_t *)&bitset1, 0), 0);
    CU_ASSERT_EQUAL(bitset_next_clear_bit((bitset_t *)&bitset1, 499), 499);
    CU_ASSERT_EQUAL(bitset_next_clear_bit((bitset_t *)&bitset1, 500), -1);

    bitset_set((bitset_t *)&bitset1, 0);

    CU_ASSERT_EQUAL(bitset_next_clear_bit((bitset_t *)&bitset1, 0), 1);
    CU_ASSERT_EQUAL(bitset_next_clear_bit((bitset_t *)&bitset1, 499), 499);
    CU_ASSERT_EQUAL(bitset_next_clear_bit((bitset_t *)&bitset1, 500), -1);

    bitset_clear((bitset_t *)&bitset1, 1000);

    CU_ASSERT_EQUAL(bitset_next_clear_bit((bitset_t *)&bitset1, 0), 1);
    CU_ASSERT_EQUAL(bitset_next_clear_bit((bitset_t *)&bitset1, 499), 499);
    CU_ASSERT_EQUAL(bitset_next_clear_bit((bitset_t *)&bitset1, 500), 1000);
}

static int bitset_test_suite_init(void)
{
    return 0;
}

static int bitset_test_suite_cleanup(void)
{
    return 0;
}

static CU_TestInfo cases[] = {
    { "bitset_init_test", bitset_init_test },
    { "bitset_size_test", bitset_size_test },
    { "bitset_set_and_clear_test", bitset_set_and_clear_test },
    { "bitset_prev_set_bit_test", bitset_prev_set_bit_test },
    { "bitset_next_set_bit_test", bitset_next_set_bit_test },
    { "bitset_prev_clear_bit_test", bitset_prev_clear_bit_test },
    { "bitset_next_clear_bit_test", bitset_next_clear_bit_test },
    { NULL, NULL }
};

static CU_SuiteInfo suite[] = {
    {   "bitset test",
        bitset_test_suite_init,
        bitset_test_suite_cleanup,
        NULL,
        NULL,
        cases
    },
    {
        NULL
    }
};

CU_SuiteInfo* bitset_test_suite_info(void)
{
    return suite;
}
