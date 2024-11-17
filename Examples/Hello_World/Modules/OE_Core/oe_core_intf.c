/**
 * @note This file was autogenerated with OpenEDOS v2.1.
 * Sections inside USER CODE BEGIN and USER CODE END will be left untouched 
 * when rerunning the code generation. Happy coding!
 * 
 * @file oe_core_intf.c
 * @author Samuel Ardaya-Lieb
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
#include "oe_config.h"
#if OE_USE_SYSTEM_REQUESTS
/* USER CODE FILE INTRODUCTION END */

#include "oe_core_intf.h"
#include "oe_core_mod.h"

/* Includes, typedefs, globals, etc. */
/* USER CODE INTERFACE GLOBALS BEGIN */

/* USER CODE INTERFACE GLOBALS END */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Requests ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

OE_Error_t req_Kernel_Start(
	OE_KernelID_t KernelID)
{
	OE_MessageHeader_t MessageHeader = {
		.RequestID = RID_Kernel_Start,
	};

	return OE_Core_sendRequest(
		&MessageHeader,
		&(struct requestArgs_Kernel_Start_s){
			KernelID,
		});
}


/* Something else...? */
/* USER CODE INTERFACE SOURCE BEGIN */
#endif
/* USER CODE INTERFACE SOURCE END */