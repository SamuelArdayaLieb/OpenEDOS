/**
 * OpenEDOS Kernel v2.0.0
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

#ifndef KERNEL_H
#define KERNEL_H

/**
 * This is the header file of the kernel. The kernel is the core object of the operating
 * system as it provides the main endless loop that runs an application. The actual
 * functionality of the application is implemented inside modules. The kernel however
 * manages the communication between these modules. An application might use more than one
 * kernels. In this case, each kernel is placed in its own task/thread.
 *
 * A kernel's job is to register message handlers and distribute messages to those handlers.
 * Messages are used to transport requests and responses. One kernel is connected to one
 * message queue (which is a FIFO). In its main routine, a kernel gets the next message from
 * the message queue and forwards it to the corresponding handlers. If the message queue
 * is empty the kernel goes to sleep and only wakes up when a new request occures.
 */

#include "defines.h"
#include "request_map.h"
#include "message_queue.h"

struct Kernel_s
{
    /* The ID that this kernel has during operation. It is set by the kernel switch. */
    KernelID_t KernelID;

    /* Connection to the kernel switch. */
    KernelSwitch_t *KernelSwitch;

    /* The request map maps request IDs to message handlers. */
    RequestMap_t RequestMap;
};

/**
 * @brief Initialize the kernel statically.
 *
 * This function initializes the request map
 * and connects the kernel to the kernel switch.
 *
 * @param Kernel The pointer to the kernel to be initialized.
 */
Error_t Kernel_staticInit(
    Kernel_t *Kernel);

/**
 * @brief Register a list of message handlers for a list of request IDs.
 *
 * This function is used by modules to register one or more handlers at the
 * kernel. This can be done at any time during operation. Once a handler is
 * registered, the kernel will execute it whenever the corresponding request
 * occures.
 *
 * @param Kernel The pointer to the kernel that will register the handlers
 * in its request map.
 *
 * @param RequestIDs Pointer to the array of request IDs.
 *
 * @param RequestHandlers Pointer to the array of message handlers to be registered.
 *
 * @param NumberOfHandlers The number of request IDs and handlers.
 *
 * @return Error_t An error is returned if
 * - registering the handlers results in an error.
 * Otherwise ERROR_NONE is returned.
 */
Error_t Kernel_registerHandlers(
    Kernel_t *Kernel,
    RequestID_t *RequestIDs,
    MessageHandler_t *RequestHandlers,
    size_t NumberOfRequests);

/**
 * @brief Unregister a list of message handlers for a list of request IDs.
 *
 * This function is the counterpart to the Kernel_registerHandlers() function.
 * It is used by a module to unregister one or more message handlers.
 * This can be done at any time during operation. Once a request handler is
 * unregistered, the kernel will no longer execute it when the request occures.
 *
 * @param Kernel The pointer to the kernel that will unregister the handlers
 * in its request map.
 *
 * @param RequestIDs Pointer to the array of request IDs.
 *
 * @param RequestHandlers Pointer to the array of message handlers to be unregistered.
 *
 * @param NumberOfHandlers The number of request IDs and handlers.
 */
void Kernel_unregisterHandlers(
    Kernel_t *Kernel,
    RequestID_t *RequestIDs,
    MessageHandler_t *RequestHandlers,
    size_t NumberOfRequests);

/**
 * @brief Checks if at least one handler is registered for a given request ID.
 *
 * @param Kernel The pointer to the kernel whose request map is checked.
 *
 * @param RequestID The request ID to be checked.
 *
 * @return true At least one handler is registered.
 *
 * @return false No handler is registered.
 */
bool Kernel_handlerRegistered(
    Kernel_t *Kernel,
    RequestID_t RequestID);

/**
 * @brief The main routine of the kernel.
 *
 * In this function the kernel does its work. The function is an endless loop
 * consisting of three parts:
 * 1. The kernel tries to get a new message from the kernel switch.
 * 2. If there is a new message, the kernel processes the message.
 * 3. If there is no new message, the kernel goes into IDLE().
 *
 * New messages are sent to the kernel switch by ISRs or modules.
 *
 * @param Kernel The pointer to the kernel that runs.
 */
void Kernel_run(
    Kernel_t *Kernel);

/**
 * @brief Run a single cycle of the kernel main routine.
 *
 * The Kernel_run() function is an endless loop consisting of three parts:
 * 1. The kernel tries to get a new message from the kernel switch.
 * 2. If there is a new message, the kernel processes the message.
 * 3. If there is no new message, the kernel goes into IDLE().
 *
 * Kernel_runOnce() does basically the same thing but not in an endless loop.
 * Instead it only runs one cycle. This means that one message is fetched and
 * processed and then the function returns.
 *
 * This function may be used in software tests where a message is placed in the
 * message queue. The behaviour of the system can then be observed step by step.
 *
 * @param Kernel The pointer to the kernel that runs once.
 */
void Kernel_runOnce(
    Kernel_t *Kernel);

/**
 * @brief Handle a new message.
 *
 * This method is called after a new message was received from the kernel switch.
 * According to the message type the message is forwarded as either a request or
 * a response.
 *
 * @param Kernel The pointer to the kernel that handles the message.
 *
 * @param Message Pointer to the received message.
 */
void Kernel_handleMessage(
    Kernel_t *Kernel,
    Message_t *Message);

#endif // KERNEL_H