/**
 * @note This file was autogenerated with OpenEDOS v2.1.
 * Sections inside USER CODE BEGIN and USER CODE END will be left untouched 
 * when rerunning the code generation. Happy coding!
 * 
 * @file printer_mod.h
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

#ifndef PRINTER_MOD_H
#define PRINTER_MOD_H

#include "oe_defines.h"
#include "timer_intf.h"

/* Includes, typedefs, globals, etc. */
/* USER CODE MODULE GLOBALS BEGIN */

/* USER CODE MODULE GLOBALS END */

//~~~~~~~~~~~~~~~~~~~~~~~~ Module definition ~~~~~~~~~~~~~~~~~~~~~~~~//

typedef struct module_Printer_s {
    /* The connection to the kernel. */
    OE_Kernel_t *Kernel;

    /* Module data. */
	/* USER CODE MODULE DATA BEGIN */

	/* USER CODE MODULE DATA END */

} module_Printer_t;

//~~~~~~~~~~~~~~~~~~~~~~~~~~ Init prototype ~~~~~~~~~~~~~~~~~~~~~~~~~//

/**
 * @brief Initialize the module Printer.
 * 
 * This function registers the request handlers of the module
 * and connects the module to the kernel. It then calls 
 * the specific init function of the module.
 * 
 * @param Printer A pointer to the module to be initialized.
 * 
 * @param Args A pointer to the init params for the module.
 * 
 * @param Kernel A pointer to the kernel to be connected.
 * 
 * @return OE_Error_t An error is returned if
 * - initializing the module results in an error.
 * Otherwise OE_ERROR_NONE is returned.
 */
OE_Error_t initModule_Printer(
    module_Printer_t *Printer,
    void *Args,
    OE_Kernel_t *Kernel);

//~~~~~~~~~~~~~~~~~~~~~~~~~~ User prototypes ~~~~~~~~~~~~~~~~~~~~~~~~//

/* USER CODE MODULE PROTOTYPES BEGIN */

/* USER CODE MODULE PROTOTYPES END */

#endif // PRINTER_MOD_H