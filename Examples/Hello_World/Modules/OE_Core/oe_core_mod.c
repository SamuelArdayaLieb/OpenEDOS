/**
 * @note This file was autogenerated with OpenEDOS v2.1.
 * Sections inside USER CODE BEGIN and USER CODE END will be left untouched 
 * when rerunning the code generation. Happy coding!
 * 
 * @file oe_core_mod.c
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

/* USER CODE FILE INTRODUCTION END */

#include "oe_core_mod.h"
#include "oe_kernel.h"

/* Includes, prototypes, globals, etc. */
/* USER CODE MODULE GLOBALS BEGIN */
#include <string.h>

#if OE_USE_REQUEST_LIMIT
/**
 * @brief Check if the request register is full.
 *
 * This function checks if the request limit of a given request ID
 * is reached in the request register of a given kernel.
 *
 * @param KernelID The kernel ID.
 *
 * @param RequestID The request ID.
 *
 * @return true Returns true if the request register is full.
 * @return false Returns false if the request register is not full.
 */
static inline bool OE_Core_registerFull(
    OE_KernelID_t KernelID,
    OE_RequestID_t RequestID);

/**
 * @brief Set an entry in the request register.
 *
 * When a new request occures it has to be registered in the request register before
 * it can be queued. This is done by increasing the current request count for the
 * given request ID. Further processing of a request only takes place if the request
 * register is not full.
 *
 * @param KernelID The ID of the kernel whose request register is used.
 *
 * @param RequestID The ID of the new request that has to be registered.
 *
 * @return OE_Error_t An error is returned if
 * - the kernel ID is invalid.
 * - the request ID is invalid.
 * - the request register is full.
 * Otherwise OE_ERROR_NONE is returned.
 */
static inline OE_Error_t OE_Core_setRequestEntry(
    OE_KernelID_t KernelID,
    OE_RequestID_t RequestID);

/**
 * @brief Clear an entry in the request register.
 *
 * This function is basically the opposite of setRequestEntry(). It is called when
 * a request message leaves the message queue.
 *
 * @param KernelID The ID of the kernel whose request register is used.
 *
 * @param RequestID ID of the request.
 */
static inline void OE_Core_clearRequestEntry(
    OE_KernelID_t KernelID,
    OE_RequestID_t RequestID);
#endif // OE_USE_REQUEST_LIMIT
/* USER CODE MODULE GLOBALS END */

/* Global pointer to the module. */
static module_OE_Core_t *OE_Core;

//~~~~~~~~~~~~~~~~~~~~~~~~ Custom init prototype ~~~~~~~~~~~~~~~~~~~~~~~~//

/**
 * @brief Custom initializer for the module: OE_Core.
 * 
 * @param Args A pointer to the init params for the module.
 * 
 * @return OE_Error_t An error is returned if
 * - initializing the module results in an error.
 * Otherwise OE_ERROR_NONE is returned.
 */
static inline OE_Error_t init_OE_Core(void *Args);

//~~~~~~~~~~~~~~~~~~~~~~ Request handler prototypes ~~~~~~~~~~~~~~~~~~~~~//

/* This module does not implement any request handlers. */

//~~~~~~~~~~~~~~~~~~~~~ Response handler prototypes ~~~~~~~~~~~~~~~~~~~~~//

/* This module does not implement any response handlers. */

//~~~~~~~~~~~~~~~~~~~~~~~~ Module initialization ~~~~~~~~~~~~~~~~~~~~~~~~//

/* Initialize the module and register handlers. */
OE_Error_t initModule_OE_Core(
    module_OE_Core_t *pOE_Core,
    void *Args,
    OE_Kernel_t *Kernel)
{
    OE_Error_t Error;
    
    /* List the requests this module will handle. */
    OE_RequestID_t RequestIDs[] = {
	};

    /* List the request handlers accordingly. */
    OE_MessageHandler_t RequestHandlers[] = {
	};

    /* Setup the module connections. */
    OE_Core = pOE_Core;
    OE_Core->Kernel = Kernel;

    /* Initialize the module. */
    Error = init_OE_Core(Args);

    /* Check for errors. */
    if (Error != OE_ERROR_NONE)
    {
        OE_Core->Kernel = NULL;
        OE_Core = NULL;
        
        return Error;
    }

    /* Register the request handlers. */
    Error = OE_Kernel_registerHandlers(
        Kernel,
        RequestIDs,
        RequestHandlers,
        sizeof(RequestIDs)/sizeof(OE_RequestID_t));

    /* Check for errors. */
    if (Error != OE_ERROR_NONE)
    {
        /* Unregister handlers if an error occured. */
        OE_Kernel_unregisterHandlers(
            Kernel,
            RequestIDs,
            RequestHandlers,
            sizeof(RequestIDs)/sizeof(OE_RequestID_t));
        
        OE_Core->Kernel = NULL;
        OE_Core = NULL;  

        return Error; 
    }

    /* Nice, we're done here. */
    return OE_ERROR_NONE;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~ Custom init function ~~~~~~~~~~~~~~~~~~~~~~~~//

OE_Error_t init_OE_Core(void *Args)
{
	/* USER CODE MODULE INIT BEGIN */
    /* Avoid unused warning. */
    (void)Args;

	OE_Core->NumberOfKernels = 0;

    for (OE_KernelID_t KernelID = 0; KernelID < OE_NUMBER_OF_KERNELS; KernelID++)
    {
#if OE_USE_REQUEST_LIMIT
        /* Init the request registers. */
        for (size_t Count = 0; Count < OE_NUMBER_OF_REQUESTS; Count++)
        {
			OE_Core->RequestRegisters[KernelID][Count].NumberOfRequests = 0;
            OE_Core->RequestRegisters[KernelID][Count].RequestLimit = OE_REQUEST_LIMIT;
        }
#endif // OE_USE_REQUEST_LIMIT

        /* Init message queue. */
        OE_MessageQueue_staticInit(
            &OE_Core->MessageQueues[KernelID]);

        OE_Core->Kernels[KernelID] = NULL;
    }

	/* Return no error if everything is fine. */
	return OE_ERROR_NONE;
	/* USER CODE MODULE INIT END */
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~ Request handlers ~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/* This module does not implement any request handlers. */

//~~~~~~~~~~~~~~~~~~~~~~~~~~ Response handlers ~~~~~~~~~~~~~~~~~~~~~~~~~~//

/* This module does not implement any response handlers. */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~ User functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/* USER CODE MODULE FUNCTIONS BEGIN */
OE_Error_t OE_Core_connectKernel(
    OE_Kernel_t *Kernel)
{
    OE_KernelID_t KernelID;
    
    OE_ENTER_CRITICAL();

    /* First, check if the kernel is already connected. */
    for (KernelID = 0; KernelID < OE_Core->NumberOfKernels; KernelID++)
    {
        if (OE_Core->Kernels[KernelID] == Kernel)
        {
            OE_EXIT_CRITICAL();

            return OE_ERROR_NONE;
        }
    }

    if (OE_Core->NumberOfKernels >= OE_NUMBER_OF_KERNELS)
    {
        OE_EXIT_CRITICAL();

        return OE_ERROR_KERNEL_LIMIT_REACHED;
    }

    Kernel->KernelID = OE_Core->NumberOfKernels;

    OE_Core->Kernels[OE_Core->NumberOfKernels] = Kernel;

    OE_Core->NumberOfKernels++;

    Kernel->Core = OE_Core;

    OE_EXIT_CRITICAL();

    return OE_ERROR_NONE;
}

OE_Error_t OE_Core_sendRequest(
    OE_MessageHeader_t *Header,
    const void *Parameters)
{
    OE_Message_t *Message = NULL;
    OE_KernelID_t KernelID;
    bool handlerRegistered = false;

    if (Header->KernelID >= OE_NUMBER_OF_KERNELS)
    {
        return OE_ERROR_KERNEL_ID_INVALID;
    }

    if (Header->RequestID >= OE_NUMBER_OF_REQUESTS)
    {
        return OE_ERROR_REQUEST_ID_INVALID;
    }

    OE_ENTER_CRITICAL();

    /* Control loop. */
    for (KernelID = 0; KernelID < OE_Core->NumberOfKernels; KernelID++)
    {
        /* Check which Kernels receive the request. */
        if (OE_Kernel_handlerRegistered(OE_Core->Kernels[KernelID], Header->RequestID))
        {
            handlerRegistered = true;
#if OE_USE_REQUEST_LIMIT
            if (OE_Core_registerFull(KernelID, Header->RequestID))
            {
                OE_EXIT_CRITICAL();

                return OE_ERROR_REQUEST_LIMIT_REACHED;
            }
#endif // OE_USE_REQUEST_LIMIT
            if (OE_MessageQueue_isFull(&OE_Core->MessageQueues[KernelID]))
            {
                OE_EXIT_CRITICAL();

                return OE_ERROR_MESSAGE_QUEUE_FULL;
            }
        }
    }

    if (!handlerRegistered)
    {
        OE_EXIT_CRITICAL();
        
        return OE_ERROR_NONE;
    }

    /* Send loop. */
    for (KernelID = 0; KernelID < OE_Core->NumberOfKernels; KernelID++)
    {
        /* Check which Kernels receive the request. */
        if (OE_Kernel_handlerRegistered(OE_Core->Kernels[KernelID], Header->RequestID))
        {
#if OE_USE_REQUEST_LIMIT
            if (OE_Core_setRequestEntry(KernelID, Header->RequestID) == OE_ERROR_NONE)
#endif // OE_USE_REQUEST_LIMIT
            {
                Message = OE_MessageQueue_allocateMessage(
                    &OE_Core->MessageQueues[KernelID]);

                if (Message == NULL)
                {
#if OE_USE_REQUEST_LIMIT
                    OE_Core_clearRequestEntry(
                        KernelID,
                        Header->RequestID);
#endif // OE_USE_REQUEST_LIMIT
                    continue;
                }

                Message->Header = *Header;
                Message->Header.Information = OE_MESSAGE_TYPE_REQUEST | OE_MESSAGE_DATA_EMPTY;

                if (Parameters != NULL)
                {
                    memcpy(
                        &(Message->Data),
                        Parameters,
                        OE_MESSAGE_DATA_SIZE);

                    Message->Header.Information &= ~(OE_MESSAGE_DATA_EMPTY);
                }

                OE_RESUME(KernelID);
            }
        }
    }

    OE_EXIT_CRITICAL();

    return OE_ERROR_NONE;
}

OE_Error_t OE_Core_sendResponse(
    OE_MessageHeader_t *Header,
    const void *Parameters)
{
    OE_Message_t *Message = NULL;

    if (Header->KernelID >= OE_NUMBER_OF_KERNELS)
    {
        return OE_ERROR_KERNEL_ID_INVALID;
    }

    if (Header->RequestID >= OE_NUMBER_OF_REQUESTS)
    {
        return OE_ERROR_REQUEST_ID_INVALID;
    }

    if (Header->ResponseHandler == NULL)
    {
        return OE_ERROR_PARAMETER_INVALID;
    }

    OE_ENTER_CRITICAL();

    Message = OE_MessageQueue_allocateMessage(
        &OE_Core->MessageQueues[Header->KernelID]);

    if (Message == NULL)
    {
        OE_EXIT_CRITICAL();

        return OE_ERROR_MESSAGE_QUEUE_FULL;
    }

    Message->Header = *Header;
    Message->Header.Information = OE_MESSAGE_DATA_EMPTY;

    if (Parameters != NULL)
    {
        memcpy(
            &(Message->Data),
            Parameters,
            OE_MESSAGE_DATA_SIZE);

        Message->Header.Information &= ~(OE_MESSAGE_DATA_EMPTY);
    }

    OE_RESUME(Header->KernelID);

    OE_EXIT_CRITICAL();

    return OE_ERROR_NONE;
}

OE_Message_t *OE_Core_getMessage(
    OE_KernelID_t KernelID)
{
    OE_Message_t *Message;

    OE_ENTER_CRITICAL();

    Message = OE_MessageQueue_getMessage(
        &OE_Core->MessageQueues[KernelID]);

    if (Message == NULL)
    {
        OE_EXIT_CRITICAL();

        return NULL;
    }

    /** 
     * If a module unsubscribed to a request after it was placed in the message queue,
     * the unsubscribed request is still in the queue. Check again to protect state machines.
     */
    if ((Message->Header.Information & OE_MESSAGE_TYPE_REQUEST)
    && (!OE_Kernel_handlerRegistered(OE_Core->Kernels[KernelID], Message->Header.RequestID)))
    {
        OE_EXIT_CRITICAL();

        return NULL;
    }

#if OE_USE_REQUEST_LIMIT
    /* If the message contains a request, we need to clear the request entry. */
    if (Message->Header.Information & OE_MESSAGE_TYPE_REQUEST)
    {
        OE_Core_clearRequestEntry(
            KernelID,
            Message->Header.RequestID);
    }
#endif // OE_USE_REQUEST_LIMIT

    OE_EXIT_CRITICAL();

    return Message;
}

#if OE_USE_REQUEST_LIMIT
bool OE_Core_registerFull(
    OE_KernelID_t KernelID,
    OE_RequestID_t RequestID)
{
    OE_RequestRegisterEntry_t *RequestEntry;

    RequestEntry = &OE_Core->RequestRegisters[KernelID][RequestID];

    if (RequestEntry->NumberOfRequests >= RequestEntry->RequestLimit)
    {
        return true;
    }

    return false;
}

OE_Error_t OE_Core_setRequestEntry(
    OE_KernelID_t KernelID,
    OE_RequestID_t RequestID)
{
    OE_RequestRegisterEntry_t *RequestEntry;

    /* This makes accessing the memory a little bit easier. */
    RequestEntry = &OE_Core->RequestRegisters[KernelID][RequestID];

    /* Check if the register is full. */
    if (RequestEntry->NumberOfRequests >= RequestEntry->RequestLimit)
    {
        return OE_ERROR_REQUEST_LIMIT_REACHED;
    }

    /* Increase the current request count. */
    RequestEntry->NumberOfRequests++;

    return OE_ERROR_NONE;
}

void OE_Core_clearRequestEntry(
    OE_KernelID_t KernelID,
    OE_RequestID_t RequestID)
{
    /* Check if the request register holds a request. */
    if (OE_Core->RequestRegisters[KernelID][RequestID].NumberOfRequests > 0)
    {
        /* Decrease the current request count. */
        OE_Core->RequestRegisters[KernelID][RequestID].NumberOfRequests--;
    }
}
#endif // OE_USE_REQUEST_LIMIT
/* USER CODE MODULE FUNCTIONS END */
