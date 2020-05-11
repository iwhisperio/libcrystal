#include <CUnit/Basic.h>

typedef CU_SuiteInfo* (*SuiteInfoFunc)(void);

typedef struct TestSuite {
    const char* fileName;
    SuiteInfoFunc getSuiteInfo;
} TestSuite;

CU_SuiteInfo* bitset_test_suite_info(void);
CU_SuiteInfo* base58_test_suite_info(void);

TestSuite suites[] = {
    { "bitset_test.c", bitset_test_suite_info },
    { "base58_test.c", base58_test_suite_info },
    { NULL, NULL}
};
