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

/* Static core as usual */
static OE_Core_t OE_Core;
/* Static kernels for testing */
static OE_Kernel_t Kernel_0, Kernel_1, Kernel_2, Kernel_3;

/* OpenEDOS threads */
static pthread_t kernel_threads[OE_NUMBER_OF_KERNELS];

pthread_mutex_t condition_mutexes[OE_NUMBER_OF_KERNELS] = { PTHREAD_MUTEX_INITIALIZER };
pthread_cond_t condition_conds[OE_NUMBER_OF_KERNELS] = { PTHREAD_COND_INITIALIZER };

/* Test threads */
#define NUM_TOGGLE_SUBSCRIPTION_THREADS 10
#define NUM_DUMMY_0_REQ_THREADS 10
#define NUM_DUMMY_1_REQ_THREADS 10
#define NUM_DUMMY_2_REQ_THREADS 10
static pthread_t toggleSubscription_threads[NUM_TOGGLE_SUBSCRIPTION_THREADS];
static pthread_t sendDummy_0_Req_threads[NUM_DUMMY_0_REQ_THREADS];
static pthread_t sendDummy_1_Req_threads[NUM_DUMMY_1_REQ_THREADS];
static pthread_t sendDummy_2_Req_threads[NUM_DUMMY_2_REQ_THREADS];

/* Test flags */
static volatile bool sendDummy_0_Request_flag;
static volatile bool sendDummy_1_Request_flag;
static volatile bool sendDummy_2_Request_flag;
static volatile bool toggleSubscription_flag;
static volatile bool handlerRegistered_flags[NUM_TOGGLE_SUBSCRIPTION_THREADS];
static volatile bool responseReceived_flags[NUM_DUMMY_0_REQ_THREADS];
static volatile uint8_t dummy_0_TestParams[NUM_DUMMY_0_REQ_THREADS]; 

struct dummy_0_threadArgs {
    CuTest *tc;
    uint8_t id;
};

#define TEST_DELAY_S  1
#define TEST_DELAY_US 1

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
    /* Never reached... */
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
    /* Never reached... */
    return NULL;
    /* USER CODE KERNEL 1 RUN END */
}

static __attribute__ ((__unused__)) void *Kernel_2_thread(void *Args)
{
    /* USER CODE KERNEL 2 INIT BEGIN */
	OE_Error_t Error;
    CuTest *tc = (CuTest*)Args;
	module_Dummy_2_t Dummy_2;

	/* Initialize all modules. */
    Error = initModule_Dummy_2(
        &Dummy_2,
        (void*)tc,
        &Kernel_2);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    /* USER CODE KERNEL 2 INIT END */
    /* USER CODE KERNEL 2 RUN BEGIN */
	/* Enter kernel main routine. */
	OE_Kernel_run(&Kernel_2);
    /* Never reached... */
    return NULL;
    /* USER CODE KERNEL 2 RUN END */
}

static void *toggleSubscription(void *Args)
{
    OE_Error_t Error;
    CuTest *tc = (CuTest*)Args;

    while (toggleSubscription_flag)
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

static void dummy_0_response(
	OE_MessageHeader_t *Header,
	struct responseArgs_Dummy_0_Req_s *Args)
{
    CuAssertIntEquals(Args->tc, Kernel_0.KernelID, Header->KernelID);
    responseReceived_flags[Args->param] = true;
}

static void *sendDummy_0_Request(void *Args)
{
    OE_Error_t Error;
    struct dummy_0_threadArgs *threadArgs = (struct dummy_0_threadArgs*)Args;
    CuTest *tc = threadArgs->tc;
    uint8_t id = threadArgs->id; 
    
    while (sendDummy_0_Request_flag)
    {
        Error = OE_ERROR_MESSAGE_QUEUE_FULL;
        while ((Error == OE_ERROR_MESSAGE_QUEUE_FULL) 
        || (Error == OE_ERROR_REQUEST_LIMIT_REACHED)
        || (!responseReceived_flags[id]))
        {
            responseReceived_flags[id] = false;
            Error = req_Dummy_0_Req(id, tc, (OE_MessageHandler_t)dummy_0_response, Kernel_0.KernelID);
        }
        CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    }

    return NULL;
}

static void *sendDummy_1_Request(void *Args)
{
    OE_Error_t Error;
    CuTest *tc = (CuTest*)Args;
    
    while (sendDummy_1_Request_flag)
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

static void *sendDummy_2_Request(void *Args)
{
    OE_Error_t Error;
    CuTest *tc = (CuTest*)Args;
    
    while (sendDummy_2_Request_flag)
    {
        Error = OE_ERROR_MESSAGE_QUEUE_FULL;
        while ((Error == OE_ERROR_MESSAGE_QUEUE_FULL) 
        || (Error == OE_ERROR_REQUEST_LIMIT_REACHED))
        {
            Error = req_Dummy_2_Req(TEST_VAL_2);
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

    responseReceived_flags[0] = true;
    /* USER CODE RESPONSE DUMMY 0 REQ END */
}

static void startKernelThreads(CuTest *tc)
{
    int Ret;
    const struct sched_param param1 = {.sched_priority = 1};
    const struct sched_param param2 = {.sched_priority = 1};
    const struct sched_param param3 = {.sched_priority = 1};

    printf("Starting kernels\n");

    Ret = pthread_create(&kernel_threads[0], NULL, Kernel_0_thread, tc);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_setschedparam(kernel_threads[0], SCHED_RR, &param1);
    CuAssertIntEquals(tc, 0, Ret);  
    
    Ret = pthread_create(&kernel_threads[1], NULL, Kernel_1_thread, tc);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_setschedparam(kernel_threads[1], SCHED_RR, &param2);
    CuAssertIntEquals(tc, 0, Ret); 
    
    Ret = pthread_create(&kernel_threads[2], NULL, Kernel_2_thread, tc);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_setschedparam(kernel_threads[2], SCHED_RR, &param3);
    CuAssertIntEquals(tc, 0, Ret);      
}

static void stopKernelThreads(CuTest *tc)
{
    int Ret;
    printf("Stopping kernels\n");

    OE_Error_t Error = req_Test_End();
    
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    
    Ret = pthread_join(kernel_threads[0], NULL);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_join(kernel_threads[1], NULL);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_join(kernel_threads[2], NULL);
    CuAssertIntEquals(tc, 0, Ret);    
}

static void __attribute__ ((__unused__)) test_multiKernel_run(CuTest *tc)
{
    printf("Starting run test\n");
        
    init(tc);

    startKernelThreads(tc);

    usleep(TEST_DELAY_US);

    stopKernelThreads(tc);
}

static void __attribute__ ((__unused__)) test_multiKernel_singleRespone(CuTest *tc)
{
    OE_Error_t Error;
    
    printf("Starting single response test\n");

    responseReceived_flags[0] = false;
    TestParam_1 = TEST_VAL_1;
    
    init(tc);

    startKernelThreads(tc);

    usleep(TEST_DELAY_US);

    Error = req_Dummy_0_Req(
        TestParam_1, tc, 
        (OE_MessageHandler_t)handleResponse_Dummy_0_Req, 
        Kernel_0.KernelID);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);

    usleep(TEST_DELAY_US);

    CuAssertTrue(tc, responseReceived_flags[0]);

    stopKernelThreads(tc);    
}

static void __attribute__ ((__unused__)) test_multiKernel_interact(CuTest *tc)
{
    int Ret, i;
    const struct sched_param param = {.sched_priority = 1};
    
    printf("Starting interact test\n");

    sendDummy_2_Request_flag = true;
    toggleSubscription_flag = true;

    init(tc);

    startKernelThreads(tc);

    usleep(TEST_DELAY_US);

    for (i=0; i < NUM_TOGGLE_SUBSCRIPTION_THREADS; i++)
    {
        handlerRegistered_flags[i] = true;

        Ret = pthread_create(&toggleSubscription_threads[i], NULL, toggleSubscription, tc);
        CuAssertIntEquals(tc, 0, Ret);
        Ret = pthread_setschedparam(toggleSubscription_threads[i], SCHED_RR, &param);
        CuAssertIntEquals(tc, 0, Ret);
    }

    for (i=0; i < NUM_DUMMY_2_REQ_THREADS; i++)
    {
        Ret = pthread_create(&sendDummy_2_Req_threads[i], NULL, sendDummy_2_Request, tc);
        CuAssertIntEquals(tc, 0, Ret);
        Ret = pthread_setschedparam(sendDummy_2_Req_threads[i], SCHED_RR, &param);
        CuAssertIntEquals(tc, 0, Ret);
    }

    sleep(TEST_DELAY_S);

    toggleSubscription_flag = false;
    sendDummy_2_Request_flag = false;

    for (i=0; i < NUM_TOGGLE_SUBSCRIPTION_THREADS; i++)
    {
        pthread_join(toggleSubscription_threads[i], NULL);
    }

    for (i=0; i < NUM_DUMMY_2_REQ_THREADS; i++)
    {
        pthread_join(sendDummy_2_Req_threads[i], NULL);
    }

    usleep(TEST_DELAY_US);

    stopKernelThreads(tc);
}

static void __attribute__ ((__unused__)) test_multiKernel_handlerRegistration(CuTest *tc)
{
    int Ret, i;
    const struct sched_param param = {.sched_priority = 1};
    
    printf("Starting handler registration test\n");

    sendDummy_1_Request_flag = true;
    toggleSubscription_flag = true;

    init(tc);

    startKernelThreads(tc);

    usleep(TEST_DELAY_US);

    for (i=0; i < NUM_TOGGLE_SUBSCRIPTION_THREADS; i++)
    {
        handlerRegistered_flags[i] = true;

        Ret = pthread_create(&toggleSubscription_threads[i], NULL, toggleSubscription, tc);
        CuAssertIntEquals(tc, 0, Ret);
        Ret = pthread_setschedparam(toggleSubscription_threads[i], SCHED_RR, &param);
        CuAssertIntEquals(tc, 0, Ret);
    }

    for (i=0; i < NUM_DUMMY_1_REQ_THREADS; i++)
    {
        Ret = pthread_create(&sendDummy_1_Req_threads[i], NULL, sendDummy_1_Request, tc);
        CuAssertIntEquals(tc, 0, Ret);
        Ret = pthread_setschedparam(sendDummy_1_Req_threads[i], SCHED_RR, &param);
        CuAssertIntEquals(tc, 0, Ret);
    }

    sleep(TEST_DELAY_S);

    toggleSubscription_flag = false;
    sendDummy_1_Request_flag = false;

    for (i=0; i < NUM_TOGGLE_SUBSCRIPTION_THREADS; i++)
    {
        pthread_join(toggleSubscription_threads[i], NULL);
    }

    for (i=0; i < NUM_DUMMY_1_REQ_THREADS; i++)
    {
        pthread_join(sendDummy_1_Req_threads[i], NULL);
    }

    usleep(TEST_DELAY_US);

    stopKernelThreads(tc);
}

static void __attribute__ ((__unused__)) test_multiKernel_response(CuTest *tc)
{
    int Ret, i;
    const struct sched_param param = {.sched_priority = 1};
    struct dummy_0_threadArgs args;
    args.tc = tc;

    printf("Starting response test\n");

    sendDummy_0_Request_flag = true;

    init(tc);

    startKernelThreads(tc);

    usleep(TEST_DELAY_US);

    for (i=0; i < NUM_DUMMY_0_REQ_THREADS; i++)
    {
        responseReceived_flags[i] = true;
        args.id = i;
        Ret = pthread_create(&sendDummy_0_Req_threads[i], NULL, sendDummy_0_Request, &args);
        CuAssertIntEquals(tc, 0, Ret);
        Ret = pthread_setschedparam(sendDummy_0_Req_threads[i], SCHED_RR, &param);
        CuAssertIntEquals(tc, 0, Ret);   
    }

    sleep(TEST_DELAY_S);

    sendDummy_0_Request_flag = false;
   
    for (i=0; i < NUM_DUMMY_0_REQ_THREADS; i++)
    {
        pthread_join(sendDummy_0_Req_threads[i], NULL);
    }

    usleep(TEST_DELAY_US);
    
    stopKernelThreads(tc);
}

void add_multiKernel(CuSuite *suite)
{
    // SUITE_ADD_TEST(suite, test_multiKernel_initModule);
    // SUITE_ADD_TEST(suite, test_multiKernel_staticInit);
    // SUITE_ADD_TEST(suite, test_multiKernel_run);
    SUITE_ADD_TEST(suite, test_multiKernel_interact);
    // SUITE_ADD_TEST(suite, test_multiKernel_handlerRegistration);
    // SUITE_ADD_TEST(suite, test_multiKernel_response);
    // SUITE_ADD_TEST(suite, test_multiKernel_singleRespone);
}