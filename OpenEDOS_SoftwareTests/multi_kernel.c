#include "multi_kernel.h"

/* OpenEDOS core */
#include "kernel_switch.h"
#include "kernel.h"

/* Modules */
#include "system_requests/system_requests_intf.h"

/* Stuff */
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

static KernelSwitch_t KernelSwitch;

static pthread_t threads[NUMBER_OF_KERNELS];

pthread_mutex_t condition_mutexes[NUMBER_OF_KERNELS] = { PTHREAD_MUTEX_INITIALIZER };
pthread_cond_t condition_conds[NUMBER_OF_KERNELS] = { PTHREAD_COND_INITIALIZER };

static void test_multiKernel_init(CuTest *tc)
{
    Kernel_t Kernel_0, Kernel_1, Kernel_2, Kernel_3;
    Error_t Error;

    KernelSwitch_staticInit(&KernelSwitch);

    Error = Kernel_staticInit(&Kernel_0);
    CuAssertIntEquals(tc, ERROR_NONE, Error);

    Error = Kernel_staticInit(&Kernel_1);
    CuAssertIntEquals(tc, ERROR_NONE, Error);

    Error = Kernel_staticInit(&Kernel_2);
    CuAssertIntEquals(tc, ERROR_NONE, Error);

    /* Not more than 3 kernel should be accepted. */
    Error = Kernel_staticInit(&Kernel_3);
    CuAssertIntEquals(tc, ERROR_KERNEL_LIMIT_REACHED, Error);

    /* Reinitalizing should be fine though. */
    Error = Kernel_staticInit(&Kernel_0);
    CuAssertIntEquals(tc, ERROR_NONE, Error);

    Error = Kernel_staticInit(&Kernel_1);
    CuAssertIntEquals(tc, ERROR_NONE, Error);

    Error = Kernel_staticInit(&Kernel_2);
    CuAssertIntEquals(tc, ERROR_NONE, Error);
    
    /* Check again. */
    Error = Kernel_staticInit(&Kernel_3);
    CuAssertIntEquals(tc, ERROR_KERNEL_LIMIT_REACHED, Error);
}

static void *Kernel_0_thread(void *args)
{
    Kernel_t Kernel;

    //module_Input_t Input;
    
    Kernel_staticInit(
        &Kernel);

    // initModule_Input(
    //     &Input, 
    //     &Kernel);

    Kernel_run(
        &Kernel);
}

static void *Kernel_1_thread(void *args)
{
    Kernel_t Kernel;

    //module_Input_t Input;
    
    Kernel_staticInit(
        &Kernel);

    // initModule_Input(
    //     &Input, 
    //     &Kernel);

    Kernel_run(
        &Kernel);
}

static void *Kernel_2_thread(void *args)
{
    Kernel_t Kernel;

    //module_Input_t Input;
    
    Kernel_staticInit(
        &Kernel);

    // initModule_Input(
    //     &Input, 
    //     &Kernel);

    Kernel_run(
        &Kernel);
}

static void test_multiKernel_run(CuTest *tc)
{
    srand(time(NULL));
    
    KernelSwitch_staticInit(
        &KernelSwitch);
    
    pthread_create(&threads[0], NULL, Kernel_0_thread, NULL);
    pthread_create(&threads[1], NULL, Kernel_1_thread, NULL);
    pthread_create(&threads[2], NULL, Kernel_2_thread, NULL);
    
    struct sched_param param = {.sched_priority = 20};

    pthread_setschedparam(threads[0], SCHED_RR, &param);
    pthread_setschedparam(threads[1], SCHED_RR, &param);
    pthread_setschedparam(threads[2], SCHED_RR, &param);

    sleep(5);

    req_Kernel_Stop(0);
    req_Kernel_Stop(1);
    req_Kernel_Stop(2);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
}


void add_multiKernel(CuSuite *suite)
{
    SUITE_ADD_TEST(suite, test_multiKernel_init);
    SUITE_ADD_TEST(suite, test_multiKernel_run);
}