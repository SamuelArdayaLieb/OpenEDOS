#include "single_kernel.h"

/* OpenEDOS core */
#include "kernel_switch.h"
#include "kernel.h"

/* Modules */
#include "system_requests/system_requests_intf.h"
#include "test_dummy_intf.h"
#include "test_dummy_mod.h"

static KernelSwitch_t KernelSwitch;

static void init(CuTest *tc, Kernel_t *Kernel)
{
    Error_t Error;
    
    TestParam_1 = TEST_VAL_TEST_BEGIN;
    TestParam_2 = TEST_VAL_TEST_BEGIN;
    TestParam_3 = TEST_VAL_TEST_BEGIN;

    KernelSwitch_staticInit(&KernelSwitch);

    Error = Kernel_staticInit(Kernel);

    CuAssertIntEquals(tc, ERROR_NONE, Error);

    /* There should be no message in the queue. */
    CuAssertIntEquals(tc, 0, 
        Kernel->KernelSwitch->MessageQueues[Kernel->KernelID].NumberOfMessages);
}

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

    init(tc, &Kernel);

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

static void test_systemStart(CuTest *tc)
{
    Kernel_t Kernel;
    module_TestDummy_t TestDummy;
    Error_t Error;

    init(tc, &Kernel);

    Error = initModule_TestDummy(
        &TestDummy,
        &Kernel);

    CuAssertIntEquals(tc, ERROR_NONE, Error);

    /* Test values should have changed. */
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_1);
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_2);
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_3); 

    /* Process system start request. */
    Error = req_System_Start(Kernel.KernelID);
    CuAssertIntEquals(tc, ERROR_NONE, Error);
    /* There should be one message in the queue. */
    CuAssertIntEquals(tc, 1, Kernel.KernelSwitch->MessageQueues[Kernel.KernelID].NumberOfMessages);

    Kernel_runOnce(&Kernel);

    CuAssertIntEquals(tc, TEST_VAL_SYSTEM_START, TestParam_1);
    CuAssertIntEquals(tc, TEST_VAL_SYSTEM_START, TestParam_2);
    CuAssertIntEquals(tc, TEST_VAL_SYSTEM_START, TestParam_3);  

    /* There should be one message in the queue. */
    CuAssertIntEquals(tc, 0, Kernel.KernelSwitch->MessageQueues[Kernel.KernelID].NumberOfMessages);
}

static void test_subscribeRequest(CuTest *tc)
{
    Kernel_t Kernel;
    module_TestDummy_t TestDummy;
    Error_t Error;

    init(tc, &Kernel);

    Error = initModule_TestDummy(
        &TestDummy,
        &Kernel);

    CuAssertIntEquals(tc, ERROR_NONE, Error);

    /* Test values should have changed. */
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_1);
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_2);
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_3); 

    /* Unsubscribe request to see if it's not handled. */
    unsubscribeRequest_1();
    Error = sendRequest_1();
    CuAssertIntEquals(tc, ERROR_NONE, Error);
    CuAssertIntEquals(tc, 0, Kernel.KernelSwitch->MessageQueues[Kernel.KernelID].NumberOfMessages);

    /* Process request. */
    Kernel_runOnce(&Kernel);

    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_1);
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_2);     
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_3);

    /* Subscribe request to see if it's handled. */
    Error = subscribeRequest_1();
    CuAssertIntEquals(tc, ERROR_NONE, Error);
    Error = sendRequest_1();
    CuAssertIntEquals(tc, ERROR_NONE, Error);
    CuAssertIntEquals(tc, 1, Kernel.KernelSwitch->MessageQueues[Kernel.KernelID].NumberOfMessages);

    /* Process request. */
    Kernel_runOnce(&Kernel);

    CuAssertIntEquals(tc, TEST_VAL_1, TestParam_1);
    CuAssertIntEquals(tc, TEST_VAL_2, TestParam_2);     
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_3);
    CuAssertIntEquals(tc, 1, Kernel.KernelSwitch->MessageQueues[Kernel.KernelID].NumberOfMessages);

    /* Process response. */
    Kernel_runOnce(&Kernel);
    CuAssertIntEquals(tc, TEST_VAL_1, TestParam_1);
    CuAssertIntEquals(tc, TEST_VAL_2, TestParam_2); 
    CuAssertIntEquals(tc, TEST_VAL_3, TestParam_3);
    CuAssertIntEquals(tc, 0, Kernel.KernelSwitch->MessageQueues[Kernel.KernelID].NumberOfMessages);
}

static void test_MessageQueue_isFull(CuTest *tc)
{
    Kernel_t Kernel;
    module_TestDummy_t TestDummy;
    Error_t Error;

    init(tc, &Kernel);

    Error = initModule_TestDummy(
        &TestDummy,
        &Kernel);
    CuAssertIntEquals(tc, ERROR_NONE, Error);    

    /* Sending one request should return no error. */
    Error = sendRequest_1();
    CuAssertIntEquals(tc, ERROR_NONE, Error);
    CuAssertIntEquals(tc, 1, Kernel.KernelSwitch->MessageQueues[Kernel.KernelID].NumberOfMessages);

    /* The message queue length is 1. Sending another request should return an error. */
    Error = sendRequest_2();
    CuAssertIntEquals(tc, ERROR_MESSAGE_QUEUE_FULL, Error);
    CuAssertIntEquals(tc, 1, Kernel.KernelSwitch->MessageQueues[Kernel.KernelID].NumberOfMessages);
}

static void test_RequestLimitReached(CuTest *tc)
{
    Kernel_t Kernel;
    module_TestDummy_t TestDummy;
    Error_t Error;

    init(tc, &Kernel);

    Error = initModule_TestDummy(
        &TestDummy,
        &Kernel);
    CuAssertIntEquals(tc, ERROR_NONE, Error);    

    /* Sending one request should return no error. */
    Error = sendRequest_1();
    CuAssertIntEquals(tc, ERROR_NONE, Error);
    CuAssertIntEquals(tc, 1, Kernel.KernelSwitch->MessageQueues[Kernel.KernelID].NumberOfMessages);

    /* The request limit is 1. Sending another request should return an error. */
    Error = sendRequest_1();
    CuAssertIntEquals(tc, ERROR_REQUEST_LIMIT_REACHED, Error);
    CuAssertIntEquals(tc, 1, Kernel.KernelSwitch->MessageQueues[Kernel.KernelID].NumberOfMessages);
}

void add_singleKernel(CuSuite *suite)
{
    SUITE_ADD_TEST(suite, test_Kernel_staticInit);
    SUITE_ADD_TEST(suite, test_initModule);
    SUITE_ADD_TEST(suite, test_systemStart);
    SUITE_ADD_TEST(suite, test_subscribeRequest);
    SUITE_ADD_TEST(suite, test_MessageQueue_isFull);
    SUITE_ADD_TEST(suite, test_RequestLimitReached);
}