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
pthread_mutex_t critical_section_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutexes[OE_NUMBER_OF_KERNELS] = { PTHREAD_MUTEX_INITIALIZER };
pthread_cond_t condition_conds[OE_NUMBER_OF_KERNELS] = { PTHREAD_COND_INITIALIZER };

atomic_bool kernel_running[OE_NUMBER_OF_KERNELS];

/* Test threads */
#define NUM_TOGGLE_SUBSCRIPTION_THREADS 0
#define NUM_DUMMY_0_REQ_THREADS 1
#define NUM_DUMMY_1_REQ_THREADS 1
#define NUM_DUMMY_2_REQ_THREADS 2
static pthread_t toggleSubscription_threads[NUM_TOGGLE_SUBSCRIPTION_THREADS];
static pthread_t sendDummy_0_Req_threads[NUM_DUMMY_0_REQ_THREADS];
static pthread_t sendDummy_1_Req_threads[NUM_DUMMY_1_REQ_THREADS];
static pthread_t sendDummy_2_Req_threads[NUM_DUMMY_2_REQ_THREADS];

/* Test flags */
static volatile atomic_bool sendDummy_0_Request_flag;
static volatile atomic_bool sendDummy_1_Request_flag;
static volatile atomic_bool sendDummy_2_Request_flag;
static volatile atomic_bool toggleSubscription_flag;
static volatile atomic_bool handlerRegistered_flags[NUM_TOGGLE_SUBSCRIPTION_THREADS];
static volatile atomic_bool responseReceived_flags[NUM_DUMMY_0_REQ_THREADS];
static volatile uint8_t dummy_0_TestParams[NUM_DUMMY_0_REQ_THREADS]; 

struct dummy_0_threadArgs {
    CuTest *tc;
    uint8_t id;
};

struct timespec ts;

#define NUM_TEST_RUNS 2000
#define TEST_DELAY_NS 100000 // 100 us

static void init(CuTest *tc)
{
    OE_Error_t Error;
    
    ts.tv_sec = 0;
    ts.tv_nsec = TEST_DELAY_NS;

    TestParam_0 = TEST_VAL_TEST_BEGIN;
    TestParam_1 = TEST_VAL_TEST_BEGIN;
    TestParam_2 = TEST_VAL_TEST_BEGIN;

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

static void Test_Kernel_run(CuTest* tc)
{
    OE_Kernel_run((OE_Kernel_t*)tc->args);
}

static __attribute__ ((__unused__)) void *Kernel_0_thread(void *Args)
{
    (void)Args;
	OE_Error_t Error;
	module_Dummy_0_t Dummy_0;
    CuSuite *suite = CuSuiteNew();
    CuTest *tc = CuTestNewArgs("Test_Kernel_0", Test_Kernel_run, (void*)&Kernel_0);

    CuSuiteAdd(suite, tc);

    Error = initModule_Dummy_0(
        &Dummy_0,
        (void*)suite,
        &Kernel_0);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);

    CuSuiteRun(suite);
    /* Never reached... */
    return NULL;
}

static __attribute__ ((__unused__)) void *Kernel_1_thread(void *Args)
{
    (void)Args;
	OE_Error_t Error;
	module_Dummy_1_t Dummy_1;
    CuSuite *suite = CuSuiteNew();
    CuTest *tc = CuTestNewArgs("Test_Kernel_1", Test_Kernel_run, (void*)&Kernel_1);
	
	CuSuiteAdd(suite, tc);

    Error = initModule_Dummy_1(
        &Dummy_1,
        (void*)suite,
        &Kernel_1);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    
    CuSuiteRun(suite);
    /* Never reached... */
    return NULL;
}

static __attribute__ ((__unused__)) void *Kernel_2_thread(void *Args)
{
    (void)Args;
	OE_Error_t Error;
	module_Dummy_2_t Dummy_2;
    CuSuite *suite = CuSuiteNew();
    CuTest *tc = CuTestNewArgs("Test_Kernel_2", Test_Kernel_run, (void*)&Kernel_2);

    CuSuiteAdd(suite, tc);

    Error = initModule_Dummy_2(
        &Dummy_2,
        (void*)suite,
        &Kernel_2);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);

    CuSuiteRun(suite);
    /* Never reached... */
    return NULL;
}

static void *toggleSubscription(void *Args)
{
    OE_Error_t Error;
    CuTest *tc = (CuTest*)Args;
    int i = NUM_TEST_RUNS;

    while (i)
    {        
        do {
            Error = req_Dummy_1_toggleRegistration();
            nanosleep(&ts, NULL);
            i--;
        } while ((Error == OE_ERROR_MESSAGE_QUEUE_FULL)
        || (Error == OE_ERROR_REQUEST_LIMIT_REACHED));

        CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    }

    return NULL;
}

static void dummy_0_response(
	OE_MessageHeader_t *Header,
	struct responseArgs_Dummy_0_Req_s *Args)
{
    (void)Header;
    atomic_store(&responseReceived_flags[Args->param], true);
}

static void *sendDummy_0_Request(void *Args)
{
    OE_Error_t Error;
    struct dummy_0_threadArgs *threadArgs = (struct dummy_0_threadArgs*)Args;
    CuTest *tc = threadArgs->tc;
    uint8_t id = threadArgs->id; 
    int i = NUM_TEST_RUNS;
    
    while (i)
    {
        do {
            atomic_store(&responseReceived_flags[id], false);
            Error = req_Dummy_0_Req(id, (OE_MessageHandler_t)dummy_0_response, 0);
            nanosleep(&ts, NULL);
            i--;
        } while ((Error == OE_ERROR_MESSAGE_QUEUE_FULL) 
        || (Error == OE_ERROR_REQUEST_LIMIT_REACHED)
        || (!atomic_load(&responseReceived_flags[id])));

        CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    }

    return NULL;
}

static void *sendDummy_1_Request(void *Args)
{
    OE_Error_t Error;
    CuTest *tc = (CuTest*)Args;
    int i = NUM_TEST_RUNS;
    
    while (i)
    {
        do {
            Error = req_Dummy_1_Req();
            nanosleep(&ts, NULL);
            i--;
        } while ((Error == OE_ERROR_MESSAGE_QUEUE_FULL) 
        || (Error == OE_ERROR_REQUEST_LIMIT_REACHED));

        CuAssertIntEquals(tc, OE_ERROR_NONE, Error);
    }

    return NULL;
}

static void *sendDummy_2_Request(void *Args)
{
    OE_Error_t Error;
    CuTest *tc = (CuTest*)Args;
    int i = NUM_TEST_RUNS;
    
    while (i)
    {
        do {
            Error = req_Dummy_2_Req(TEST_VAL_2);
            nanosleep(&ts, NULL);
            i--;
        } while ((Error == OE_ERROR_MESSAGE_QUEUE_FULL) 
        || (Error == OE_ERROR_REQUEST_LIMIT_REACHED));

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
    atomic_store(&responseReceived_flags[0],true);
    /* USER CODE RESPONSE DUMMY 0 REQ END */
}

static void startKernelThreads(CuTest *tc)
{
    struct sched_param param;
    pthread_attr_t attr;
    int Ret;

    printf("Starting kernels\n");
    param.sched_priority = 2;
    // Initialize the attribute object
    Ret = pthread_attr_init(&attr);
    CuAssertIntEquals(tc, 0, Ret);

    // Set the scheduling policy to SCHED_RR
    Ret = pthread_attr_setschedpolicy(&attr, SCHED_RR);
    CuAssertIntEquals(tc, 0, Ret);

    // Set the scheduling priority
    Ret = pthread_attr_setschedparam(&attr, &param);
    CuAssertIntEquals(tc, 0, Ret);

    // Set the attribute to explicitly use the scheduling parameters
    Ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    CuAssertIntEquals(tc, 0, Ret);

    // Create the thread with the configured attributes
    Ret = pthread_create(&kernel_threads[0], &attr, Kernel_0_thread, tc);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_create(&kernel_threads[1], &attr, Kernel_1_thread, tc);
    CuAssertIntEquals(tc, 0, Ret);
    Ret = pthread_create(&kernel_threads[2], &attr, Kernel_2_thread, tc);
    CuAssertIntEquals(tc, 0, Ret);

    // Destroy the attribute object
    Ret = pthread_attr_destroy(&attr);
    CuAssertIntEquals(tc, 0, Ret); 
    
    nanosleep(&ts, NULL);
}

static void stopKernelThreads(CuTest *tc)
{
    int Ret;
    printf("Stopping kernels\n");

    nanosleep(&ts, NULL);
    
    CuAssertIntEquals(tc, OE_ERROR_NONE, req_Test_End());
    
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

    stopKernelThreads(tc);
}

static void __attribute__ ((__unused__)) test_multiKernel_singleRespone(CuTest *tc)
{
    OE_Error_t Error;
    
    printf("Starting single response test\n");

    atomic_store(&responseReceived_flags[0],false);
    
    init(tc);

    startKernelThreads(tc);

    Error = req_Dummy_0_Req(
        TEST_VAL_1, 
        (OE_MessageHandler_t)handleResponse_Dummy_0_Req, 
        Kernel_0.KernelID);
    CuAssertIntEquals(tc, OE_ERROR_NONE, Error);

    nanosleep(&ts, NULL);

    CuAssertTrue(tc, atomic_load(&responseReceived_flags[0]));

    stopKernelThreads(tc);    
}

static void __attribute__ ((__unused__)) test_multiKernel_interact(CuTest *tc)
{
    int Ret, i;
    const struct sched_param param = {.sched_priority = 2};
    
    printf("Starting interact test\n");

    atomic_store(&sendDummy_2_Request_flag, true);
    atomic_store(&toggleSubscription_flag, true);
    init(tc);

    startKernelThreads(tc);

    for (i=0; i < NUM_TOGGLE_SUBSCRIPTION_THREADS; i++)
    {
        atomic_store(&handlerRegistered_flags[i], true);

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
    
    atomic_store(&toggleSubscription_flag, false);
    atomic_store(&sendDummy_2_Request_flag, false);

    for (i=0; i < NUM_TOGGLE_SUBSCRIPTION_THREADS; i++)
    {
        pthread_join(toggleSubscription_threads[i], NULL);
    }

    for (i=0; i < NUM_DUMMY_2_REQ_THREADS; i++)
    {
        pthread_join(sendDummy_2_Req_threads[i], NULL);
    }

    stopKernelThreads(tc);
}

static void __attribute__ ((__unused__)) test_multiKernel_handlerRegistration(CuTest *tc)
{
    int Ret, i;
    const struct sched_param param = {.sched_priority = 2};
    
    printf("Starting handler registration test\n");

    atomic_store(&sendDummy_1_Request_flag, true);
    atomic_store(&toggleSubscription_flag, true);

    init(tc);

    startKernelThreads(tc);

    for (i=0; i < NUM_TOGGLE_SUBSCRIPTION_THREADS; i++)
    {
        atomic_store(&handlerRegistered_flags[i], true);

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

    atomic_store(&toggleSubscription_flag, false);
    atomic_store(&sendDummy_1_Request_flag, false);

    for (i=0; i < NUM_TOGGLE_SUBSCRIPTION_THREADS; i++)
    {
        pthread_join(toggleSubscription_threads[i], NULL);
    }

    for (i=0; i < NUM_DUMMY_1_REQ_THREADS; i++)
    {
        pthread_join(sendDummy_1_Req_threads[i], NULL);
    }

    stopKernelThreads(tc);
}

static void __attribute__ ((__unused__)) test_multiKernel_response(CuTest *tc)
{
    int Ret, i;
    const struct sched_param param = {.sched_priority = 2};
    struct dummy_0_threadArgs args;
    args.tc = tc;

    printf("Starting response test\n");

    atomic_store(&sendDummy_0_Request_flag, true);

    init(tc);

    startKernelThreads(tc);

    for (i=0; i < NUM_DUMMY_0_REQ_THREADS; i++)
    {
        atomic_store(&responseReceived_flags[i], true);
        args.id = i;
        Ret = pthread_create(&sendDummy_0_Req_threads[i], NULL, sendDummy_0_Request, &args);
        CuAssertIntEquals(tc, 0, Ret);
        Ret = pthread_setschedparam(sendDummy_0_Req_threads[i], SCHED_RR, &param);
        CuAssertIntEquals(tc, 0, Ret);   
    }

    atomic_store(&sendDummy_0_Request_flag, false);
   
    for (i=0; i < NUM_DUMMY_0_REQ_THREADS; i++)
    {
        pthread_join(sendDummy_0_Req_threads[i], NULL);
    }
    
    stopKernelThreads(tc);
}

void add_multiKernel(CuSuite *suite)
{
    //SUITE_ADD_TEST(suite, test_multiKernel_staticInit);
    //SUITE_ADD_TEST(suite, test_multiKernel_run);
    SUITE_ADD_TEST(suite, test_multiKernel_interact);
    //SUITE_ADD_TEST(suite, test_multiKernel_handlerRegistration);
    //SUITE_ADD_TEST(suite, test_multiKernel_response);
    //SUITE_ADD_TEST(suite, test_multiKernel_singleRespone);
}