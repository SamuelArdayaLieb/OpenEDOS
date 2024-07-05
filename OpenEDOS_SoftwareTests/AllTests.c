#include "AllTests.h"

/* CuTest includes */
#include "CuTest.h"
#include <stdio.h>

/* OpenEDOS core */
#include "kernel_switch.h"
#include "kernel.h"

/* Modules */
#include "system_requests/system_requests_intf.h"
#include "test_dummy_intf.h"
#include "test_dummy_mod.h"

uint8_t TestParam_1;
uint8_t TestParam_2;
uint8_t TestParam_3;

static KernelSwitch_t KernelSwitch;

static void test_Kernel_staticInit(CuTest *tc)
{
    Kernel_t Kernel_1, Kernel_2;
    Error_t Error;

    KernelSwitch_staticInit(
        &KernelSwitch);
    
    /**
     * According to the OpenEDOS config, the switch works with one kernel.
     * Initializing the kernel should return no error.
     */
    Error = Kernel_staticInit(
        &Kernel_1);

    CuAssertIntEquals(tc, ERROR_NONE, Error);

    /* Initializing a second kernel should return an error. */
    Error = Kernel_staticInit(
        &Kernel_2);

    CuAssertIntEquals(tc, ERROR_KERNEL_LIMIT_REACHED, Error);

    /* Reinitializing the same kernel should again return no error. */
    Error = Kernel_staticInit(
        &Kernel_1);

    CuAssertIntEquals(tc, ERROR_NONE, Error);  
}

static void test_initModule(CuTest *tc)
{
    Kernel_t Kernel;
    module_TestDummy_t TestDummy;
    Error_t Error;

    KernelSwitch_staticInit(
        &KernelSwitch);

    Error = Kernel_staticInit(
        &Kernel);

    CuAssertIntEquals(tc, ERROR_NONE, Error);

    Error = initModule_TestDummy(
        &TestDummy,
        &Kernel);

    CuAssertIntEquals(tc, ERROR_NONE, Error);    

    /* Reinitializing the same module should not return an error. */
    Error = initModule_TestDummy(
        &TestDummy,
        &Kernel);

    CuAssertIntEquals(tc, ERROR_NONE, Error);  
}

static void test_subscribeRequest(CuTest *tc)
{
    Kernel_t Kernel;
    module_TestDummy_t TestDummy;
    Error_t Error;

    TestParam_1 = TEST_VAL_TEST_BEGIN;
    TestParam_2 = TEST_VAL_TEST_BEGIN;
    TestParam_3 = TEST_VAL_TEST_BEGIN;

    KernelSwitch_staticInit(
        &KernelSwitch);

    Error = Kernel_staticInit(
        &Kernel);

    CuAssertIntEquals(tc, ERROR_NONE, Error);

    Error = initModule_TestDummy(
        &TestDummy,
        &Kernel);

    CuAssertIntEquals(tc, ERROR_NONE, Error);

    /* Test values should have changed. */
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_1);
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_2);
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_3); 

    /* Process system start request. */
    req_System_Start(Kernel.KernelID);
    Kernel_runOnce(&Kernel);

    CuAssertIntEquals(tc, TEST_VAL_SYSTEM_START, TestParam_1);
    CuAssertIntEquals(tc, TEST_VAL_SYSTEM_START, TestParam_2);
    CuAssertIntEquals(tc, TEST_VAL_SYSTEM_START, TestParam_3);  

    /* Process request. */
    Kernel_runOnce(&Kernel);

    CuAssertIntEquals(tc, TEST_VAL_1, TestParam_1);
    CuAssertIntEquals(tc, TEST_VAL_2, TestParam_2);     
    CuAssertIntEquals(tc, TEST_VAL_SYSTEM_START, TestParam_3);

    /* Process response. */
    Kernel_runOnce(&Kernel);
    CuAssertIntEquals(tc, TEST_VAL_3, TestParam_3);
}

void RunAllTests(void) 
{
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();
    
    SUITE_ADD_TEST(suite, test_Kernel_staticInit);
    SUITE_ADD_TEST(suite, test_initModule);
    SUITE_ADD_TEST(suite, test_subscribeRequest);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
}

int main(void) 
{    
    RunAllTests();

    return 0;
}