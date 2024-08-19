/**
 * This file was autogenerated with OpenEDOS v2.0.
 * Sections inside USER CODE BEGIN and USER CODE END will be left untouched 
 * when rerunning the code generation. Happy coding!
 * 
 * @file oe_core_intf.c
 * @author Samuel Ardaya-Lieb
 * @version 2.0
 * @date 2024-07-24
 * 
 * OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT License
 * 
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 */

/* USER CODE FILE HEADER BEGIN */
#include "oe_config.h"
#if OE_USE_SYSTEM_REQUESTS
/* USER CODE FILE HEADER END */

#include "oe_core_intf.h"
#include "oe_core_mod.h"

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
