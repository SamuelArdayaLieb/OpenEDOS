/* CuTest includes */
#include "CuTest.h"
#include <stdio.h>

/* OpenEDOS includes */
#include "kernel_switch.h"
#include "kernel.h"

static KernelSwitch_t KernelSwitch;

void test_Kernel_staticInit(CuTest *tc)
{
    Kernel_t Kernel;
    Error_t Error;

    KernelSwitch_staticInit(
        &KernelSwitch);
    
    Error = Kernel_staticInit(
        &Kernel);

    CuAssertIntEquals(
        tc,
        ERROR_NONE,
        Error);
}

void RunAllTests(void) 
{
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();
    
    SUITE_ADD_TEST(suite, test_Kernel_staticInit);

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