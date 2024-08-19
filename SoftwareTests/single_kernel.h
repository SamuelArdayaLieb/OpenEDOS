#ifndef SINGEL_KERNEL_H
#define SINGLE_KERNEL_H

#include "CuTest.h"
#include <stdint.h>

#define TEST_VAL_TEST_BEGIN   0
#define TEST_VAL_MODULE_INIT  1
#define TEST_VAL_SYSTEM_START 2
#define TEST_VAL_1  0xAF
#define TEST_VAL_2  0xFE
#define TEST_VAL_3  0xFF

extern uint8_t TestParam_1;
extern uint8_t TestParam_2;
extern uint8_t TestParam_3;

void add_singleKernel(CuSuite *suite);

#endif//SINGEL_KERNEL_H