/**
 * OpenEDOS Core v0.2.1
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

#include "include/kernel_switch.h"
#include "include/kernel.h"
#include <string.h>

/* We need one global pointer to the kernel switch. */
static KernelSwitch_t *KernelSwitch;

#if USE_REQUEST_LIMIT
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
static inline bool KernelSwitch_registerFull(
    KernelID_t KernelID,
    RequestID_t RequestID);

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
 * @return Error_t An error is returned if
 * - the kernel ID is invalid.
 * - the request ID is invalid.
 * - the request register is full.
 * Otherwise ERROR_NONE is returned.
 */
static inline Error_t KernelSwitch_setRequestEntry(
    KernelID_t KernelID,
    RequestID_t RequestID);

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
static inline void KernelSwitch_clearRequestEntry(
    KernelID_t KernelID,
    RequestID_t RequestID);
#endif // USE_REQUEST_LIMIT

void KernelSwitch_staticInit(
    KernelSwitch_t *pKernelSwitch)
{
    KernelID_t KernelID;

    KernelSwitch = pKernelSwitch;
    KernelSwitch->NumberOfKernels = 0;

    for (KernelID = 0; KernelID < NUMBER_OF_KERNELS; KernelID++)
    {
#if USE_REQUEST_LIMIT
        /* Init the request registers. */
        for (size_t Count = 0; Count < NUMBER_OF_REQUESTS; Count++)
        {
            KernelSwitch->RequestRegisters[KernelID][Count].NumberOfRequests = 0;
            KernelSwitch->RequestRegisters[KernelID][Count].RequestLimit = REQUEST_LIMIT;
        }
#endif // USE_REQUEST_LIMIT

        /* Init message queue. */
        MessageQueue_staticInit(
            &KernelSwitch->MessageQueues[KernelID]);

        KernelSwitch->Kernels[KernelID] = NULL;
    }
}

Error_t KernelSwitch_connectKernel(
    Kernel_t *Kernel)
{
    if (KernelSwitch->NumberOfKernels >= NUMBER_OF_KERNELS)
    {
        return ERROR_KERNEL_LIMIT_REACHED;
    }

    ENTER_CRITICAL();

    Kernel->KernelID = KernelSwitch->NumberOfKernels;

    KernelSwitch->Kernels[KernelSwitch->NumberOfKernels] = Kernel;

    KernelSwitch->NumberOfKernels++;

    Kernel->KernelSwitch = KernelSwitch;

    EXIT_CRITICAL();

    return ERROR_NONE;
}

Error_t KernelSwitch_sendRequest(
    MessageHeader_t *Header,
    const void *Parameters)
{
    Message_t *Message = NULL;
    KernelID_t KernelID;

    if (Header->KernelID >= NUMBER_OF_KERNELS)
    {
        return ERROR_KERNEL_ID_INVALID;
    }

    if (Header->RequestID >= NUMBER_OF_REQUESTS)
    {
        return ERROR_REQUEST_ID_INVALID;
    }

    ENTER_CRITICAL();

    /* Control loop. */
    for (KernelID = 0; KernelID < KernelSwitch->NumberOfKernels; KernelID++)
    {
        /* Check which Kernels receive the request. */
        if (Kernel_handlerRegistered(KernelSwitch->Kernels[KernelID], Header->RequestID))
        {
#if USE_REQUEST_LIMIT
            if (KernelSwitch_registerFull(KernelID, Header->RequestID))
            {
                EXIT_CRITICAL();

                return ERROR_REQUEST_LIMIT_REACHED;
            }
#endif // USE_REQUEST_LIMIT
            if (MessageQueue_isFull(&KernelSwitch->MessageQueues[KernelID]))
            {
                EXIT_CRITICAL();

                return ERROR_MESSAGE_QUEUE_FULL;
            }
        }
    }

    /* Send loop. */
    for (KernelID = 0; KernelID < KernelSwitch->NumberOfKernels; KernelID++)
    {
        /* Check which Kernels receive the request. */
        if (Kernel_handlerRegistered(KernelSwitch->Kernels[KernelID], Header->RequestID))
        {
#if USE_REQUEST_LIMIT
            if (KernelSwitch_setRequestEntry(KernelID, Header->RequestID) == ERROR_NONE)
#endif // USE_REQUEST_LIMIT
            {
                Message = MessageQueue_allocateMessage(
                    &KernelSwitch->MessageQueues[KernelID]);

                if (Message == NULL)
                {
#if USE_REQUEST_LIMIT
                    KernelSwitch_clearRequestEntry(
                        KernelID,
                        Header->RequestID);
#endif // USE_REQUEST_LIMIT
                    continue;
                }

                Message->Header = *Header;
                Message->Header.Information = MESSAGE_TYPE_REQUEST | MESSAGE_DATA_EMPTY;

                if (Parameters != NULL)
                {
                    memcpy(
                        &(Message->Data),
                        Parameters,
                        MESSAGE_DATA_SIZE);

                    Message->Header.Information &= ~(MESSAGE_DATA_EMPTY);
                }

                RESUME(KernelID);
            }
        }
    }

    EXIT_CRITICAL();

    return ERROR_NONE;
}

Error_t KernelSwitch_sendResponse(
    MessageHeader_t *Header,
    const void *Parameters)
{
    Message_t *Message = NULL;

    if (Header->KernelID >= NUMBER_OF_KERNELS)
    {
        return ERROR_KERNEL_ID_INVALID;
    }

    if (Header->RequestID >= NUMBER_OF_REQUESTS)
    {
        return ERROR_REQUEST_ID_INVALID;
    }

    if (Header->ResponseHandler == NULL)
    {
        return ERROR_PARAMETER_INVALID;
    }

    ENTER_CRITICAL();

    Message = MessageQueue_allocateMessage(
        &KernelSwitch->MessageQueues[Header->KernelID]);

    if (Message == NULL)
    {
        EXIT_CRITICAL();

        return ERROR_MESSAGE_QUEUE_FULL;
    }

    Message->Header = *Header;
    Message->Header.Information = MESSAGE_DATA_EMPTY;

    if (Parameters != NULL)
    {
        memcpy(
            &(Message->Data),
            Parameters,
            MESSAGE_DATA_SIZE);

        Message->Header.Information &= ~(MESSAGE_DATA_EMPTY);
    }

    RESUME(Header->KernelID);

    EXIT_CRITICAL();

    return ERROR_NONE;
}

Message_t *KernelSwitch_getMessage(
    KernelID_t KernelID)
{
    Message_t *Message;

    ENTER_CRITICAL();

    Message = MessageQueue_getMessage(
        &KernelSwitch->MessageQueues[KernelID]);

    if (Message == NULL)
    {
        EXIT_CRITICAL();

        return NULL;
    }

#if USE_REQUEST_LIMIT
    /* If the message contains a request, we need to clear the request entry. */
    if (Message->Header.Information & MESSAGE_TYPE_REQUEST)
    {
        KernelSwitch_clearRequestEntry(
            KernelID,
            Message->Header.RequestID);
    }
#endif // USE_REQUEST_LIMIT

    EXIT_CRITICAL();

    return Message;
}

#if USE_REQUEST_LIMIT
bool KernelSwitch_registerFull(
    KernelID_t KernelID,
    RequestID_t RequestID)
{
    RequestRegisterEntry_t *RequestEntry;

    RequestEntry = &KernelSwitch->RequestRegisters[KernelID][RequestID];

    if (RequestEntry->NumberOfRequests >= RequestEntry->RequestLimit)
    {
        return true;
    }

    return false;
}

Error_t KernelSwitch_setRequestEntry(
    KernelID_t KernelID,
    RequestID_t RequestID)
{
    RequestRegisterEntry_t *RequestEntry;

    /* This makes accessing the memory a little bit easier. */
    RequestEntry = &KernelSwitch->RequestRegisters[KernelID][RequestID];

    /* Check if the register is full. */
    if (RequestEntry->NumberOfRequests >= RequestEntry->RequestLimit)
    {
        return ERROR_REQUEST_LIMIT_REACHED;
    }

    /* Increase the current request count. */
    RequestEntry->NumberOfRequests++;

    return ERROR_NONE;
}

void KernelSwitch_clearRequestEntry(
    KernelID_t KernelID,
    RequestID_t RequestID)
{
    /* Check if the request register holds a request. */
    if (KernelSwitch->RequestRegisters[KernelID][RequestID].NumberOfRequests > 0)
    {
        /* Decrease the current request count. */
        KernelSwitch->RequestRegisters[KernelID][RequestID].NumberOfRequests--;
    }
}
#endif // USE_REQUEST_LIMIT