#include <CUnit/Basic.h>

typedef CU_SuiteInfo* (*SuiteInfoFunc)(void);

typedef struct TestSuite {
    const char* fileName;
    SuiteInfoFunc getSuiteInfo;
} TestSuite;

CU_SuiteInfo* bitset_test_suite_info(void);

TestSuite suites[] = {
    { "bitset_test.c", bitset_test_suite_info },
    { NULL, NULL}
};
