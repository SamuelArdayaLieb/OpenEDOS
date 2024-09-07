#ifndef SINGEL_KERNEL_H
#define SINGLE_KERNEL_H

#include "CuTest.h"
#include <stdint.h>

#define TEST_VAL_TEST_BEGIN   0
#define TEST_VAL_MODULE_INIT  1
#define TEST_VAL_KERNEL_START 2

enum TestValues_e {
    TEST_VAL_1,
    TEST_VAL_2,
    TEST_VAL_3,
};


extern uint8_t TestParam_1;
extern uint8_t TestParam_2;
extern uint8_t TestParam_3;

void add_singleKernel(CuSuite *suite);

#endif//SINGEL_KERNEL_H