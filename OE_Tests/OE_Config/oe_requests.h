/**
 * @note This file was autogenerated with OpenEDOS v2.1.
 * Sections inside USER CODE BEGIN and USER CODE END will be left untouched 
 * when rerunning the code generation. Happy coding!
 * 
 * @file oe_requests.h
 * @author Samu, Samuel Ardaya-Lieb
 */

/* USER CODE COPYRIGHT NOTICE BEGIN */
/**
 * @version 0.0.1
 */
/* USER CODE COPYRIGHT NOTICE END */

/* USER CODE FILE INTRODUCTION BEGIN */

/* USER CODE FILE INTRODUCTION END */

#ifndef OE_REQUESTS_H
#define OE_REQUESTS_H

/**
 * @brief This file is used to create a unique ID for each request that exists in the
 * system. In order to create the ID, the request is listed in the following enum.
 */

/* Define all requests by giving them a request ID. */
typedef enum OE_RequestID_e
{
#if OE_USE_SYSTEM_REQUESTS
    /* OpenEDOS Core */
    RID_Kernel_Start,
#endif

	/* TestDummy */
	RID_Dummy_Request_1,
	RID_Dummy_Request_2,

	/* The LAST element in this enum MUST be "OE_NUMBER_OF_REQESTS"! */
    OE_NUMBER_OF_REQUESTS
} OE_RequestID_t;

#endif // OE_REQUESTS_H