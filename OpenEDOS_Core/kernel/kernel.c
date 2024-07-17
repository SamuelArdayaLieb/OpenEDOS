/**
 * OpenEDOS Core v0.2.2
 *
 * Copyright (c) 2022-2024 Samuel Ardaya-Lieb
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 *
 */

#include "include/kernel.h"
#include "include/kernel_switch.h"
#include <string.h>

Error_t Kernel_staticInit(
    Kernel_t *Kernel)
{
    /* Init the request map. */
    RequestMap_staticInit(
        &(Kernel->RequestMap));

    /* Connect the kernel to the kernel switch. */
    return KernelSwitch_connectKernel(Kernel);
}

Error_t Kernel_registerHandlers(
    Kernel_t *Kernel,
    RequestID_t *RequestIDs,
    MessageHandler_t *RequestHandlers,
    size_t NumberOfRequests)
{
    return RequestMap_registerHandlers(
        &(Kernel->RequestMap),
        RequestIDs,
        RequestHandlers,
        NumberOfRequests);
}

void Kernel_unregisterHandlers(
    Kernel_t *Kernel,
    RequestID_t *RequestIDs,
    MessageHandler_t *RequestHandlers,
    size_t NumberOfRequests)
{
    RequestMap_unregisterHandlers(
        &(Kernel->RequestMap),
        RequestIDs,
        RequestHandlers,
        NumberOfRequests);
}

bool Kernel_handlerRegistered(
    Kernel_t *Kernel,
    RequestID_t RequestID)
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

void Kernel_run(
    Kernel_t *Kernel)
{
    Message_t *Message = NULL;

    if (Kernel->KernelID >= NUMBER_OF_KERNELS)
    {
        return;
    }

#if USE_SYSTEM_REQUESTS
    req_Kernel_Start(Kernel->KernelID);
#endif

    while (true)
    {
        Message = KernelSwitch_getMessage(Kernel->KernelID);

        if (Message == NULL)
        {
            IDLE(Kernel->KernelID);
        }
        else
        {
            Kernel_handleMessage(
                Kernel,
                Message);
        }
    }
}

void Kernel_runOnce(
    Kernel_t *Kernel)
{
    Message_t *Message = NULL;

    if (Kernel->KernelID >= NUMBER_OF_KERNELS)
    {
        return;
    }

    Message = KernelSwitch_getMessage(Kernel->KernelID);

    if (Message != NULL)
    {
        Kernel_handleMessage(
            Kernel,
            Message);
    }
}

void Kernel_handleMessage(
    Kernel_t *Kernel,
    Message_t *Message)
{
    if (Message->Header.Information & MESSAGE_TYPE_REQUEST)
    {
        RequestMapNode_t *MapNode = RequestMap_getHandlers(
            &(Kernel->RequestMap),
            Message->Header.RequestID);

        if (MapNode == NULL)
        {
            return;
        }

        for (size_t Count = 0; Count < MapNode->NumberOfHandlers; Count++)
        {
            if (Message->Header.Information & MESSAGE_DATA_EMPTY)
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
    else
    {
        if (Message->Header.Information & MESSAGE_DATA_EMPTY)
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