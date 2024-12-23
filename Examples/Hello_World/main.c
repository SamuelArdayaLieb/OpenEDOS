/**
 * @note This file was autogenerated with OpenEDOS v2.1.
 * Sections inside USER CODE BEGIN and USER CODE END will be left untouched 
 * when rerunning the code generation. Happy coding!
 * 
 * @file main.c
 * @author Samuel Ardaya-Lieb
 */

/* USER CODE COPYRIGHT NOTICE BEGIN */
/**
 * @version 1.0
 * 
 * (c) 2024 Samuel Ardaya-Lieb, MIT license
 */
/* USER CODE COPYRIGHT NOTICE END */

/* USER CODE FILE INTRODUCTION BEGIN */

/* USER CODE FILE INTRODUCTION END */

#include "oe_core_mod.h"
#include "oe_kernel.h"
#include "oe_all_modules.h"

/* Includes, prototypes, globals, etc. */
/* USER CODE MAIN GLOBALS BEGIN */

/* USER CODE MAIN GLOBALS END */

/* Kernel threads. */
void Kernel_0_thread(void *Args);

static OE_Core_t OE_Core;

int main(void)
{
	/* USER CODE MAIN FUNCTION BEGIN */
	initModule_OE_Core(&OE_Core, NULL, NULL);

	/* Call the one thread function (no multi threading). */
	Kernel_0_thread(NULL);
	/* USER CODE MAIN FUNCTION END */

	/* Never reached... */
	return 0;
}

void Kernel_0_thread(void *Args)
{
	/* USER CODE KERNEL 0 INIT BEGIN */
	OE_Error_t Error;
	void *ModuleArgs;
	OE_Kernel_t Kernel_0;

	module_Timer_t Timer;
	module_Printer_t Printer;

	/* Avoid unused warning. */
	(void)Args;

	/* Initialize kernel. */
	Error = OE_Kernel_staticInit(&Kernel_0);
	if (Error != OE_ERROR_NONE)
    {
        /* Error handling or debugging... */
        return;
    }

	/* Initialize all modules. */
	ModuleArgs = NULL;
    Error = initModule_Timer(
        &Timer,
        ModuleArgs,
        &Kernel_0);
    if (Error != OE_ERROR_NONE)
    {
        /* Error handling or debugging... */
        return;
    }

	ModuleArgs = NULL;
    Error = initModule_Printer(
        &Printer,
        ModuleArgs,
        &Kernel_0);
    if (Error != OE_ERROR_NONE)
    {
        /* Error handling or debugging... */
        return;
    }

	/* USER CODE KERNEL 0 INIT END */
	/* USER CODE KERNEL 0 RUN BEGIN */
	/* Enter kernel main routine. */
	OE_Kernel_run(&Kernel_0);
	/* USER CODE KERNEL 0 RUN END */
}

/* USER CODE MAIN SOURCE BEGIN */

/* USER CODE MAIN SOURCE END */
