#include "multi_kernel.h"
#include "oe_test.h"

/* OpenEDOS core */
#include "oe_core_mod.h"
#include "oe_kernel.h"

/* Modules */
#include "oe_core_intf.h"
#include "dummy_0_mod.h"
#include "dummy_0_intf.h"
#include "dummy_1_mod.h"
#include "dummy_1_intf.h"
#include "dummy_2_mod.h"
#include "dummy_2_intf.h"

/* Stuff */
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

static OE_Core_t OE_Core;
static OE_Kernel_t Kernel_0, Kernel_1, Kernel_2, Kernel_3;

static pthread_t threads[OE_NUMBER_OF_KERNELS];

pthread_mutex_t condition_mutexes[OE_NUMBER_OF_KERNELS] = { PTHREAD_MUTEX_INITIALIZER };
pthread_cond_t condition_conds[OE_NUMBER_OF_KERNELS] = { PTHREAD_COND_INITIALIZER };

static void init(CuTest *tc)
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

    /* There should be no message in the queue. */
    CuAssertIntEquals(tc, 0, 
        Kernel_0.Core->MessageQueues[Kernel_0.KernelID].NumberOfMessages);
    CuAssertIntEquals(tc, 0, 
        Kernel_1.Core->MessageQueues[Kernel_1.KernelID].NumberOfMessages);
    CuAssertIntEquals(tc, 0, 
        Kernel_2.Core->MessageQueues[Kernel_2.KernelID].NumberOfMessages);
}

static void test_multiKernel_staticInit(CuTest *tc)
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

static void test_multiKernel_initModule(CuTest *tc)
{
    (void)tc;
    
    /**
     * @TODO: Implement test!
     */
}

static void *Kernel_0_thread(void *Args)
{
    /* USER CODE KERNEL 0 INIT BEGIN */
	OE_Error_t Error;
	void *ModuleArgs;
	OE_Kernel_t Kernel_0;

	module_Dummy_0_t Dummy_0;

	/* Avoid unused warning. */
	(void)Args;

	/* Initialize kernel. */
	Error = OE_Kernel_staticInit(&Kernel_0);
	if (Error != OE_ERROR_NONE)
    {
        /* Error handling or debugging... */
        return NULL;
    }

	/* Initialize all modules. */
	ModuleArgs = NULL;
    Error = initModule_Dummy_0(
        &Dummy_0,
        ModuleArgs,
        &Kernel_0);
    if (Error != OE_ERROR_NONE)
    {
        /* Error handling or debugging... */
        return NULL;
    }

    /* USER CODE KERNEL 0 INIT END */
    /* USER CODE KERNEL 0 RUN BEGIN */
	/* Enter kernel main routine. */
	OE_Kernel_run(&Kernel_0);

    return NULL;
    /* USER CODE KERNEL 0 RUN END */
}

static void *Kernel_1_thread(void *Args)
{
    /* USER CODE KERNEL 1 INIT BEGIN */
	OE_Error_t Error;
	void *ModuleArgs;
	OE_Kernel_t Kernel_1;

	module_Dummy_1_t Dummy_1;

	/* Avoid unused warning. */
	(void)Args;

	/* Initialize kernel. */
	Error = OE_Kernel_staticInit(&Kernel_1);
	if (Error != OE_ERROR_NONE)
    {
        /* Error handling or debugging... */
        return NULL;
    }

	/* Initialize all modules. */
	ModuleArgs = NULL;
    Error = initModule_Dummy_1(
        &Dummy_1,
        ModuleArgs,
        &Kernel_1);
    if (Error != OE_ERROR_NONE)
    {
        /* Error handling or debugging... */
        return NULL;
    }

    /* USER CODE KERNEL 1 INIT END */
    /* USER CODE KERNEL 1 RUN BEGIN */
	/* Enter kernel main routine. */
	OE_Kernel_run(&Kernel_1);

    return NULL;
    /* USER CODE KERNEL 1 RUN END */
}

static void *Kernel_2_thread(void *Args)
{
    /* USER CODE KERNEL 2 INIT BEGIN */
	OE_Error_t Error;
	void *ModuleArgs;
	OE_Kernel_t Kernel_2;

	module_Dummy_2_t Dummy_2;

	/* Avoid unused warning. */
	(void)Args;

	/* Initialize kernel. */
	Error = OE_Kernel_staticInit(&Kernel_2);
	if (Error != OE_ERROR_NONE)
    {
        /* Error handling or debugging... */
        return NULL;
    }

	/* Initialize all modules. */
	ModuleArgs = NULL;
    Error = initModule_Dummy_2(
        &Dummy_2,
        ModuleArgs,
        &Kernel_2);
    if (Error != OE_ERROR_NONE)
    {
        /* Error handling or debugging... */
        return NULL;
    }

    /* USER CODE KERNEL 2 INIT END */
    /* USER CODE KERNEL 2 RUN BEGIN */
	/* Enter kernel main routine. */
	OE_Kernel_run(&Kernel_2);

    return NULL;
    /* USER CODE KERNEL 2 RUN END */
}

static void test_multiKernel_run(CuTest *tc)
{
    srand(time(NULL));
    
    init(tc);
    
    pthread_create(&threads[0], NULL, Kernel_0_thread, NULL);
    pthread_create(&threads[1], NULL, Kernel_1_thread, NULL);
    pthread_create(&threads[2], NULL, Kernel_2_thread, NULL);
    
    struct sched_param param = {.sched_priority = 20};

    pthread_setschedparam(threads[0], SCHED_RR, &param);
    pthread_setschedparam(threads[1], SCHED_RR, &param);
    pthread_setschedparam(threads[2], SCHED_RR, &param);

    sleep(1);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
}


void add_multiKernel(CuSuite *suite)
{
    SUITE_ADD_TEST(suite, test_multiKernel_staticInit);
    SUITE_ADD_TEST(suite, test_multiKernel_run);
}