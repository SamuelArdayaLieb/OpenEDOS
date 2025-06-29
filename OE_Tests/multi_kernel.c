/* OpenEDOS kernel */
#include "oe_kernel.h"

/* Modules */
#include "oe_all_modules.h"

/* Threading */
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

/* Testing */
#include "oe_test.h"
#include "multi_kernel.h"
#include <time.h>
#include <stdio.h>

#define TEST_DELAY_US 1

static OE_Core_t OE_Core;
static OE_Kernel_t Kernel_0, Kernel_1, Kernel_2, Kernel_3;

/* OpenEDOS threads */
static pthread_t threads[OE_NUMBER_OF_KERNELS];

pthread_mutex_t condition_mutexes[OE_NUMBER_OF_KERNELS] = { PTHREAD_MUTEX_INITIALIZER };
pthread_cond_t condition_conds[OE_NUMBER_OF_KERNELS] = { PTHREAD_COND_INITIALIZER };

/* Test threads */
static pthread_t toggleRegistration_thread;
static pthread_t toggleRegistration_thread2;
static pthread_t sendRequest_thread;
static pthread_t sendRequest_thread2;



static volatile bool responseReceived;
static volatile bool handlerRegistered;
static volatile bool sendRequests;
static volatile bool toggle;

static void init(CuTest *tc)
{
    OE_Error_t Error;
    
    TestParam_1 = TEST_VAL_TEST_BEGIN;
    TestParam_2 = TEST_VAL_TEST_BEGIN;
    TestParam_3 = TEST_VAL_TEST_BEGIN;

    initModule_OE_Core(&OE_Core, NULL, NULL);

    Error = OE_Kernel_staticInit(&Kernel_0);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 0, Kernel_0.KernelID);
    
    Error = OE_Kernel_staticInit(&Kernel_1);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 1, Kernel_1.KernelID);

    Error = OE_Kernel_staticInit(&Kernel_2);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 2, Kernel_2.KernelID);    

    /* There should be no message in the queue. */
    CuAssertIntEquals(tc, 0, 
        Kernel_0.Core->MessageQueues[Kernel_0.KernelID].NumberOfMessages);
    CuAssertIntEquals(tc, 0, 
        Kernel_1.Core->MessageQueues[Kernel_1.KernelID].NumberOfMessages);
    CuAssertIntEquals(tc, 0, 
        Kernel_2.Core->MessageQueues[Kernel_2.KernelID].NumberOfMessages);
}

static __attribute__ ((__unused__)) void test_multiKernel_staticInit(CuTest *tc)
{
    OE_Error_t Error;

    initModule_OE_Core(&OE_Core, NULL, NULL);

    Error = OE_Kernel_staticInit(&Kernel_0);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 0, Kernel_0.KernelID);

    Error = OE_Kernel_staticInit(&Kernel_1);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 1, Kernel_1.KernelID);

    Error = OE_Kernel_staticInit(&Kernel_2);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 2, Kernel_2.KernelID);

    /* Not more than 3 kernels should be accepted. */
    Error = OE_Kernel_staticInit(&Kernel_3);
    CuAssertIntEquals(tc, OE_ERROR_KERNEL_LIMIT_REACHED, Error);

    /* Reinitalizing should be fine though. */
    Error = OE_Kernel_staticInit(&Kernel_0);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 0, Kernel_0.KernelID);

    Error = OE_Kernel_staticInit(&Kernel_1);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 1, Kernel_1.KernelID);

    Error = OE_Kernel_staticInit(&Kernel_2);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    CuAssertIntEquals(tc, 2, Kernel_2.KernelID);
    
    /* Check again. */
    Error = OE_Kernel_staticInit(&Kernel_3);
    CuAssertIntEquals(tc, OE_ERROR_KERNEL_LIMIT_REACHED, Error);
}

static void __attribute__ ((__unused__)) test_multiKernel_initModule(CuTest *tc)
{
    (void)tc;
    
    /**
     * @TODO: Implement test!
     */
}

static __attribute__ ((__unused__)) void *Kernel_0_thread(void *Args)
{
    /* USER CODE KERNEL 0 INIT BEGIN */
	OE_Error_t Error;
	void *ModuleArgs;
    CuTest *tc = (CuTest*)Args;

	module_Dummy_0_t Dummy_0;

	/* Avoid unused warning. */
	(void)Args;

	/* Initialize all modules. */
	ModuleArgs = NULL;
    Error = initModule_Dummy_0(
        &Dummy_0,
        ModuleArgs,
        &Kernel_0);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    /* USER CODE KERNEL 0 INIT END */
    /* USER CODE KERNEL 0 RUN BEGIN */
	/* Enter kernel main routine. */
	OE_Kernel_run(&Kernel_0);

    return NULL;
    /* USER CODE KERNEL 0 RUN END */
}

static __attribute__ ((__unused__)) void *Kernel_1_thread(void *Args)
{
    /* USER CODE KERNEL 1 INIT BEGIN */
	OE_Error_t Error;
	void *ModuleArgs;
    CuTest *tc = (CuTest*)Args;

	module_Dummy_1_t Dummy_1;

	/* Avoid unused warning. */
	(void)Args;

	/* Initialize all modules. */
	ModuleArgs = NULL;
    Error = initModule_Dummy_1(
        &Dummy_1,
        ModuleArgs,
        &Kernel_1);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    /* USER CODE KERNEL 1 INIT END */
    /* USER CODE KERNEL 1 RUN BEGIN */
	/* Enter kernel main routine. */
	OE_Kernel_run(&Kernel_1);

    return NULL;
    /* USER CODE KERNEL 1 RUN END */
}

static __attribute__ ((__unused__)) void *Kernel_2_thread(void *Args)
{
    /* USER CODE KERNEL 2 INIT BEGIN */
	OE_Error_t Error;
	void *ModuleArgs;
    CuTest *tc = (CuTest*)Args;

	module_Dummy_2_t Dummy_2;

	/* Avoid unused warning. */
	(void)Args;

	/* Initialize all modules. */
	ModuleArgs = NULL;
    Error = initModule_Dummy_2(
        &Dummy_2,
        ModuleArgs,
        &Kernel_2);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    /* USER CODE KERNEL 2 INIT END */
    /* USER CODE KERNEL 2 RUN BEGIN */
	/* Enter kernel main routine. */
	OE_Kernel_run(&Kernel_2);

    return NULL;
    /* USER CODE KERNEL 2 RUN END */
}

static void *toggleRegistration(void *Args)
{
    OE_Error_t Error;
    CuTest *tc = (CuTest*)Args;

    while (toggle)
    {
        Error = OE_ERROR_MESSAGE_QUEUE_FULL;
        while ((Error == OE_ERROR_MESSAGE_QUEUE_FULL) || (Error == OE_ERROR_REQUEST_LIMIT_REACHED))
        {
            Error = req_Dummy_1_toggleRegistration(tc);
        }
        CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    }

    return NULL;
}

static void *sendRequest(void *Args)
{
    OE_Error_t Error;
    CuTest *tc = (CuTest*)Args;
    
    while (sendRequests)
    {
        Error = OE_ERROR_MESSAGE_QUEUE_FULL;
        while ((Error == OE_ERROR_MESSAGE_QUEUE_FULL) || (Error == OE_ERROR_REQUEST_LIMIT_REACHED))
        {
            Error = req_Dummy_1_Req(tc);
        }
        CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    }

    return NULL;
}

void handleResponse_Dummy_0_Req(
	OE_MessageHeader_t *Header,
	struct responseArgs_Dummy_0_Req_s *Args)
{
    /* USER CODE RESPONSE DUMMY 0 REQ BEGIN */
    (void)Header;
    
    CuAssertIntEquals(Args->tc, TestParam_1, Args->param);

    responseReceived = true;
    /* USER CODE RESPONSE DUMMY 0 REQ END */
}

static void startThreads(CuTest *tc)
{
    int Ret;
    const struct sched_param param1 = {.sched_priority = 1};
    const struct sched_param param2 = {.sched_priority = 1};
    const struct sched_param param3 = {.sched_priority = 1};

    Ret = pthread_create(&threads[0], NULL, Kernel_0_thread, tc);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_setschedparam(threads[0], SCHED_RR, &param1);
    CuAssertIntEquals(tc, 0, Ret);  
    
    Ret = pthread_create(&threads[1], NULL, Kernel_1_thread, tc);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_setschedparam(threads[1], SCHED_RR, &param2);
    CuAssertIntEquals(tc, 0, Ret); 
    
    Ret = pthread_create(&threads[2], NULL, Kernel_2_thread, tc);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_setschedparam(threads[2], SCHED_RR, &param3);
    CuAssertIntEquals(tc, 0, Ret);      
}

static void stopThreads(CuTest *tc)
{
    int Ret;
    OE_Error_t Error = req_Test_End();
    
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    
    Ret = pthread_join(threads[0], NULL);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_join(threads[1], NULL);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_join(threads[2], NULL);
    CuAssertIntEquals(tc, 0, Ret);    
}

static void __attribute__ ((__unused__)) test_multiKernel_run(CuTest *tc)
{
    printf("Starting run test\n");
    
    srand(time(NULL));
    
    init(tc);

    startThreads(tc);

    usleep(TEST_DELAY_US);

    stopThreads(tc);

    printf("Ending run test\n");
}

static void __attribute__ ((__unused__)) test_multiKernel_interact(CuTest *tc)
{
    OE_Error_t Error;
    
    srand(time(NULL));
    
    responseReceived = false;
    TestParam_1 = TEST_VAL_1;
    Error = OE_ERROR_MESSAGE_QUEUE_FULL;
    
    init(tc);

    startThreads(tc);

    usleep(TEST_DELAY_US);

    Error = req_Dummy_0_Req(
        TestParam_1, tc, 
        (OE_MessageHandler_t)handleResponse_Dummy_0_Req, 
        Kernel_0.KernelID);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);

    usleep(TEST_DELAY_US);

    CuAssertTrue(tc, responseReceived);

    stopThreads(tc);
}

static void __attribute__ ((__unused__)) test_multiKernel_handlerRegistration(CuTest *tc)
{
    int Ret;
    const struct sched_param param = {.sched_priority = 2};
    
    handlerRegistered = true;
    sendRequests = true;
    toggle = true;

    init(tc);

    startThreads(tc);

    usleep(TEST_DELAY_US);

    //printf("\n\nStarting registration toggle\n");
    Ret = pthread_create(&toggleRegistration_thread, NULL, toggleRegistration, tc);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_setschedparam(toggleRegistration_thread, SCHED_RR, &param);
    CuAssertIntEquals(tc, 0, Ret);

    Ret = pthread_create(&toggleRegistration_thread2, NULL, toggleRegistration, tc);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_setschedparam(toggleRegistration_thread2, SCHED_RR, &param);
    CuAssertIntEquals(tc, 0, Ret);

    Ret = pthread_create(&sendRequest_thread, NULL, sendRequest, tc);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_setschedparam(sendRequest_thread, SCHED_RR, &param);
    CuAssertIntEquals(tc, 0, Ret);

    Ret = pthread_create(&sendRequest_thread2, NULL, sendRequest, tc);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_setschedparam(sendRequest_thread2, SCHED_RR, &param);
    CuAssertIntEquals(tc, 0, Ret);

    sleep(1);

    //printf("\n\nJoining threads\n");
    toggle = false;
    sendRequests = false;
    pthread_join(toggleRegistration_thread, NULL);
    pthread_join(sendRequest_thread, NULL);

    pthread_join(toggleRegistration_thread2, NULL);
    pthread_join(sendRequest_thread2, NULL);

    usleep(TEST_DELAY_US);

    //printf("Stopping threads\n");
    stopThreads(tc);
}

void add_multiKernel(CuSuite *suite)
{
    SUITE_ADD_TEST(suite, test_multiKernel_initModule);
    SUITE_ADD_TEST(suite, test_multiKernel_staticInit);
    SUITE_ADD_TEST(suite, test_multiKernel_run);
    SUITE_ADD_TEST(suite, test_multiKernel_interact);
    SUITE_ADD_TEST(suite, test_multiKernel_handlerRegistration);
}