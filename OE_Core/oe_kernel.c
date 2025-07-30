/**
 * OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT License
 * 
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 */

#include "oe_kernel.h"
#include "oe_core_mod.h"
#include "oe_core_intf.h"

OE_Error_t OE_Kernel_staticInit(
    OE_Kernel_t *Kernel)
{
    /* Init the request map. */
    OE_RequestMap_staticInit(
        &(Kernel->RequestMap));

    /* Connect the kernel to the core. */
    return OE_Core_connectKernel(Kernel);
}

OE_Error_t OE_Kernel_registerHandlers(
    OE_Kernel_t *Kernel,
    OE_RequestID_t *RequestIDs,
    OE_MessageHandler_t *RequestHandlers,
    size_t NumberOfRequests)
{
    return OE_RequestMap_registerHandlers(
        Kernel,
        RequestIDs,
        RequestHandlers,
        NumberOfRequests);
}

void OE_Kernel_unregisterHandlers(
    OE_Kernel_t *Kernel,
    OE_RequestID_t *RequestIDs,
    OE_MessageHandler_t *RequestHandlers,
    size_t NumberOfRequests)
{
    OE_RequestMap_unregisterHandlers(
        Kernel,
        RequestIDs,
        RequestHandlers,
        NumberOfRequests);
}

bool OE_Kernel_handlerRegistered(
    OE_Kernel_t *Kernel,
    OE_RequestID_t RequestID)
{
    if (Kernel->RequestMap.MapNodes[RequestID].NumberOfHandlers > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void OE_Kernel_run(
    OE_Kernel_t *Kernel)
{
    OE_Message_t *Message = NULL;

    if (Kernel->KernelID >= OE_NUMBER_OF_KERNELS)
    {
        return;
    }

#if OE_USE_SYSTEM_REQUESTS
    if (req_Kernel_Start(Kernel->KernelID) != OE_ERROR_NONE)
    {
        return;
    }
#endif

    while (true)
    {
        Message = OE_Core_getMessage(Kernel->KernelID);

        if (Message == NULL)
        {
            OE_IDLE(Kernel->KernelID);
        }
        else
        {
            OE_Kernel_handleMessage(
                Kernel,
                Message);
        }
    }
}

bool OE_Kernel_runOnce(
    OE_Kernel_t *Kernel)
{
    OE_Message_t *Message = NULL;

    if (Kernel->KernelID >= OE_NUMBER_OF_KERNELS)
    {
        return false;
    }

    Message = OE_Core_getMessage(Kernel->KernelID);

    if (Message != NULL)
    {
        OE_Kernel_handleMessage(
            Kernel,
            Message);

        return true;
    }

    return false;
}

void OE_Kernel_handleMessage(
    OE_Kernel_t *Kernel,
    OE_Message_t *Message)
{
    if (Message->Header.Information & OE_MESSAGE_TYPE_REQUEST) // Request
    {
        OE_RequestMapNode_t *MapNode = OE_RequestMap_getHandlers(
            &(Kernel->RequestMap),
            Message->Header.RequestID);

        if (MapNode == NULL)
        {
            return;
        }

        for (size_t Count = 0; Count < MapNode->NumberOfHandlers; Count++)
        {
            if (Message->Header.Information & OE_MESSAGE_DATA_EMPTY)
            {
                /* There is no message data. */
                MapNode->RequestHandlers[Count](
                    &(Message->Header));
            }
            else
            {
                /* Message data is present. */
                MapNode->RequestHandlers[Count](
                    &(Message->Header),
                    Message->Data);
            }
        }
    }
    else // Response
    {
        if (Message->Header.Information & OE_MESSAGE_DATA_EMPTY)
        {
            /* There is no message data. */
            Message->Header.ResponseHandler(
                &(Message->Header));
        }
        else
        {
            /* Message data is present. */
            Message->Header.ResponseHandler(
                &(Message->Header),
                Message->Data);
        }
    }
}