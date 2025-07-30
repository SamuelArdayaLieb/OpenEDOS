#include "oe_test.h"

/* OpenEDOS core */
#include "oe_core_mod.h"
#include "oe_kernel.h"

/* Modules */
#include "oe_core_intf.h"
#include "test_dummy_intf.h"
#include "test_dummy_mod.h"

static OE_Core_t Core;

static void init(CuTest *tc, OE_Kernel_t *Kernel)
{
    OE_Error_t Error;
    
    TestParam_0 = TEST_VAL_TEST_BEGIN;
    TestParam_1 = TEST_VAL_TEST_BEGIN;
    TestParam_2 = TEST_VAL_TEST_BEGIN;

    initModule_OE_Core(&Core, NULL, NULL);

    Error = OE_Kernel_staticInit(Kernel);
    
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 0, Kernel->KernelID);

    /* There should be no message in the queue. */
    CuAssertIntEquals(tc, 0, 
        Kernel->Core->MessageQueues[Kernel->KernelID].NumberOfMessages);
}

static void test_singleKernel_staticInit(CuTest *tc)
{
    OE_Kernel_t Kernel;
    OE_Error_t Error;

    initModule_OE_Core(&Core, NULL, NULL);
    
    /**
     * According to the OpenEDOS config, the core works with one kernel.
     * Initializing the kernel should return no error.
     */
    Error = OE_Kernel_staticInit(&Kernel);

    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 0, Kernel.KernelID);

    /* Reinitializing the same kernel should return no error. */
    Error = OE_Kernel_staticInit(&Kernel);

    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 0, Kernel.KernelID);  
}

static void test_singleKernel_initModule(CuTest *tc)
{
    OE_Error_t Error;
    OE_Kernel_t Kernel;

    module_TestDummy_t TestDummy;
    
    init(tc, &Kernel);

    Error = initModule_TestDummy(
        &TestDummy,
        NULL,
        &Kernel);

    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);    

    /* Reinitializing the same module should not return an error. */
    Error = initModule_TestDummy(
        &TestDummy,
        NULL,
        &Kernel);

    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);  
}

static void test_singleKernel_kernelStart(CuTest *tc)
{
    OE_Kernel_t Kernel;
    module_TestDummy_t TestDummy;
    OE_Error_t Error;

    init(tc, &Kernel);

    Error = initModule_TestDummy(
        &TestDummy,
        NULL,
        &Kernel);

    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);

    /* Test values should have changed. */
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_0);
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_1);
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_2); 

    /* Send kernel start request. */
    Error = req_Kernel_Start(Kernel.KernelID);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    /* There should be one message in the queue. */
    CuAssertIntEquals(tc, 1, Kernel.Core->MessageQueues[Kernel.KernelID].NumberOfMessages);

    OE_Kernel_runOnce(&Kernel);

    CuAssertIntEquals(tc, TEST_VAL_KERNEL_START, TestParam_0);
    CuAssertIntEquals(tc, TEST_VAL_KERNEL_START, TestParam_1);
    CuAssertIntEquals(tc, TEST_VAL_KERNEL_START, TestParam_2);  

    /* There should be one message in the queue. */
    CuAssertIntEquals(tc, 0, Kernel.Core->MessageQueues[Kernel.KernelID].NumberOfMessages);
}

static void test_singleKernel_subscribeRequest(CuTest *tc)
{
    OE_Kernel_t Kernel;
    module_TestDummy_t TestDummy;
    OE_Error_t Error;
    
    init(tc, &Kernel);

    Error = initModule_TestDummy(
        &TestDummy,
        NULL,
        &Kernel);

    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);

    /* Test values should have changed. */
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_0);
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_1);
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_2); 

    /* Unsubscribe request to see if it's not handled. */
    unsubscribeRequest_1();
    Error = sendRequest_1();
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 0, Kernel.Core->MessageQueues[Kernel.KernelID].NumberOfMessages);

    /* Process request. */
    OE_Kernel_runOnce(&Kernel);

    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_0);
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_1);     
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_2);

    /* Subscribe request to see if it's handled. */
    Error = subscribeRequest_1();
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    Error = sendRequest_1();
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 1, Kernel.Core->MessageQueues[Kernel.KernelID].NumberOfMessages);

    /* Process request. */
    OE_Kernel_runOnce(&Kernel);

    CuAssertIntEquals(tc, TEST_VAL_1, TestParam_0);
    CuAssertIntEquals(tc, TEST_VAL_2, TestParam_1);     
    CuAssertIntEquals(tc, TEST_VAL_MODULE_INIT, TestParam_2);
    CuAssertIntEquals(tc, 1, Kernel.Core->MessageQueues[Kernel.KernelID].NumberOfMessages);

    /* Process response. */
    OE_Kernel_runOnce(&Kernel);
    CuAssertIntEquals(tc, TEST_VAL_1, TestParam_0);
    CuAssertIntEquals(tc, TEST_VAL_2, TestParam_1); 
    CuAssertIntEquals(tc, TEST_VAL_3, TestParam_2);
    CuAssertIntEquals(tc, 0, Kernel.Core->MessageQueues[Kernel.KernelID].NumberOfMessages);
}

static void __attribute__ ((__unused__)) test_singleKernel_messageQueueFull(CuTest *tc)
{
    OE_Kernel_t Kernel;
    module_TestDummy_t TestDummy;
    OE_Error_t Error;

    init(tc, &Kernel);

    Error = initModule_TestDummy(
        &TestDummy,
        NULL,
        &Kernel);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);    

    /* Sending one request should return no error. */
    Error = sendRequest_1();
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 1, Kernel.Core->MessageQueues[Kernel.KernelID].NumberOfMessages);

    /* The message queue length is 1. Sending another request should return an error. */
    Error = sendRequest_2();
    CuAssertIntEquals(tc, OE_ERROR_MESSAGE_QUEUE_FULL, Error);
    CuAssertIntEquals(tc, 1, Kernel.Core->MessageQueues[Kernel.KernelID].NumberOfMessages);
}

static void test_singleKernel_requestLimitReached(CuTest *tc)
{
    OE_Kernel_t Kernel;
    module_TestDummy_t TestDummy;
    OE_Error_t Error;
    int i = 0;

    init(tc, &Kernel);

    Error = initModule_TestDummy(
        &TestDummy,
        NULL,
        &Kernel);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);    

    while (i++ < OE_REQUEST_LIMIT)
    {
        Error = sendRequest_1();
        CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
        CuAssertIntEquals(tc, i, Kernel.Core->MessageQueues[Kernel.KernelID].NumberOfMessages);
    }

    /* The request limit is reached. Sending another request should return an error. */
    Error = sendRequest_1();
    CuAssertIntEquals(tc, OE_ERROR_REQUEST_LIMIT_REACHED, Error);
    CuAssertIntEquals(tc, OE_REQUEST_LIMIT, Kernel.Core->MessageQueues[Kernel.KernelID].NumberOfMessages);
}

void add_singleKernel(CuSuite *suite)
{
    SUITE_ADD_TEST(suite, test_singleKernel_staticInit);
    SUITE_ADD_TEST(suite, test_singleKernel_initModule);
    SUITE_ADD_TEST(suite, test_singleKernel_kernelStart);
    SUITE_ADD_TEST(suite, test_singleKernel_subscribeRequest);
    //SUITE_ADD_TEST(suite, test_singleKernel_messageQueueFull);
    SUITE_ADD_TEST(suite, test_singleKernel_requestLimitReached);
}