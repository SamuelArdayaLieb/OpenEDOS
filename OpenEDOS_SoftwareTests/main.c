#include "single_kernel.h"
#include "multi_kernel.h"
#include <stdio.h>

uint8_t TestParam_1;
uint8_t TestParam_2;
uint8_t TestParam_3;

int main(void) 
{    
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();
    
    add_singleKernel(suite);
    add_multiKernel(suite);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);

    return 0;
}