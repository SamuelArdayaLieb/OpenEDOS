/**
 * OpenEDOS Config: oe_requests.h
 * 
 * @version 2.0.0
 * 
 * Copyright (c) 2022-2024 Samuel Ardaya-Lieb, MIT License
 *
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 */

#ifndef OE_REQUESTS_H
#define OE_REQUESTS_H

/**
 * @brief This file is used to create a unique ID for each request that exists in the
 * system. To create the ID, simply list the request in the following enum. All
 * requests have to be listed here.
 */

#include "oe_config.h"

typedef enum OE_RequestID_e
{
    /**
     * List your requests here by giving them a meaningful name.
     * For example:
     * RID_MyCoolRequest,
     */

#if OE_USE_SYSTEM_REQUESTS
    /* OpenEDOS Core */
    RID_Kernel_Start,
#endif

    /* Test Dummy */
    RID_Dummy_Request_1,
    RID_Dummy_Request_2,

    /* The LAST element in this enum MUST be "OE_NUMBER_OF_REQESTS"! */
    OE_NUMBER_OF_REQUESTS
} OE_RequestID_t;

#endif // OE_REQUESTS_H