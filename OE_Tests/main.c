/**
 * @note This file was autogenerated with OpenEDOS v2.1.
 * Sections inside USER CODE BEGIN and USER CODE END will be left untouched 
 * when rerunning the code generation. Happy coding!
 * 
 * @file main.c
 * @author Samu, Samuel Ardaya-Lieb
 */

/* USER CODE COPYRIGHT NOTICE BEGIN */
/**
 * @version 2.1
 * 
 * OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT License
 * 
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 */
/* USER CODE COPYRIGHT NOTICE END */

/* USER CODE FILE INTRODUCTION BEGIN */

/* USER CODE FILE INTRODUCTION END */

#include "oe_core_mod.h"
#include "oe_kernel.h"
#include "oe_all_modules.h"

/* Includes, prototypes, globals, etc. */
/* USER CODE MAIN GLOBALS BEGIN */
#include "single_kernel.h"
#include "multi_kernel.h"
#include <stdio.h>

uint8_t TestParam_1;
uint8_t TestParam_2;
uint8_t TestParam_3;
/* USER CODE MAIN GLOBALS END */

/* Kernel threads. */
void Kernel_0_thread(void *Args);

static OE_Core_t OE_Core;

int main(void)
{
	/* USER CODE MAIN FUNCTION BEGIN */
    /* Avoid unused warning. */
    (void)OE_Core;
    
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();
    
    add_singleKernel(suite);
    add_multiKernel(suite);

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
	/* USER CODE MAIN FUNCTION END */

	/* Never reached... */
	return 0;
}

void Kernel_0_thread(void *Args)
{
	/* USER CODE KERNEL 0 INIT BEGIN */
    
	/* USER CODE KERNEL 0 INIT END */
	/* USER CODE KERNEL 0 RUN BEGIN */

	/* USER CODE KERNEL 0 RUN END */
}

/* USER CODE MAIN SOURCE BEGIN */

/* USER CODE MAIN SOURCE END */
