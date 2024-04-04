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

#ifndef KERNEL_SWITCH_H
#define KERNEL_SWITCH_H

/**
 * This file describes the kernel switch. The kernel switch is responsible for
 * receiving messages and forwarding them to the correct kernels. It also checks
 * the request limit for each request. The kernel switch holds one message queue
 * for each kernel and sends messages addressed to a kernel to its queue.
 */

#include "defines.h"
#include "message_queue.h"

struct KernelSwitch_s
{
    /* Kernel connections */
    Kernel_t *Kernels[NUMBER_OF_KERNELS];
    size_t NumberOfKernels;

#if USE_REQUEST_LIMIT
    /**
     * The request register serves to keep track of how many requests are queued at a time.
     * There is one register entry for each request ID.
     */
    RequestRegisterEntry_t RequestRegisters[NUMBER_OF_KERNELS][NUMBER_OF_REQUESTS];
#endif // USE_REQUEST_LIMIT

    /* Message queues */
    MessageQueue_t MessageQueues[NUMBER_OF_KERNELS];
}; // The typedef for KernelSwitch_t is done in defines.h.

/**
 * @brief Initialize the kernel switch statically.
 *
 * This function initializes the kernel connections, the request registers
 * and the message queues. A kernel switch has to be provided.
 *
 * @param KernelSwitch Pointer to the kernel switch that will be initialized.
 */
void KernelSwitch_staticInit(
    KernelSwitch_t *KernelSwitch);

/**
 * @brief Connect a kernel to the kernel switch.
 *
 * @param Kernel Pointer to the kernel to be connected with the kernel switch.
 *
 * @return Error_t An error is returned if
 * - the number of kernels has reached its limit.
 * Otherwise ERROR_NONE is returned.
 */
Error_t KernelSwitch_connectKernel(
    Kernel_t *Kernel);

/**
 * @brief Send a request message to the kernel switch.
 *
 * This function sends a request message to the kernel switch.
 * The kernel switch checks which kernels receive the request.
 * It then allocates a message in each message queue and copies
 * the given data. A valid message header has to be provided.
 *
 * @param Header Pointer to the message header.
 *
 * @param Parameters Pointer to the paramters transported in the message.
 *
 * @return Error_t An error is returned if
 * - the kernel ID is invalid.
 * - the request ID is invalid.
 * - the request register is full.
 * - the message queue is full.
 * Otherwise ERROR_NONE is returned.
 */
Error_t KernelSwitch_sendRequest(
    MessageHeader_t *Header,
    const void *Parameters);

/**
 * @brief Send a response message to the kernel switch.
 *
 * This function sends a response message to the kernel switch.
 * The kernel switch allocates a message in the message queue of the
 * destination kernel and writes the the given data. A valid message
 * header has to be provided.
 *
 * @param Header Pointer to the message header.
 *
 * @param Parameters Pointer to the paramters transported in the message.
 *
 * @return Error_t An error is returned if
 * - the kernel ID is invalid.
 * - the request ID is invalid.
 * - the response handler is invalid.
 * - the message queue is full.
 * Otherwise ERROR_NONE is returned.
 */
Error_t KernelSwitch_sendResponse(
    MessageHeader_t *Header,
    const void *Parameters);

/**
 * @brief Get the next message from the message queue.
 *
 * This method is called inside the kernel main routine.
 * It is used to get the next message from the message queue.
 *
 * @param KernelID The ID of the kernel that calls the function.
 *
 * @return Message_t* A pointer to the next message is returned.
 * If the message queue is empty, NULL is returned.
 */
Message_t* KernelSwitch_getMessage(
    KernelID_t KernelID);

#endif // KERNEL_SWITCH_H