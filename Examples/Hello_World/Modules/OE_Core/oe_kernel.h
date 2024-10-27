/**
 * OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT License
 * 
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 */

#ifndef OE_KERNEL_H
#define OE_KERNEL_H

/**
 * @brief This is the header file of the kernel. The kernel is the central object of the 
 * operating system. It provides the main endless loop that runs an application. The actual
 * functionality of the application is implemented inside modules. The kernel however
 * manages the communication between those modules. An application might use more than one
 * kernel. In this case, each kernel is placed in its own task/thread.
 *
 * A kernel's job is to register message handlers and distribute messages to those handlers.
 * Messages are used to transport requests and responses. One kernel is connected to one
 * message queue (which is a FIFO). In its main routine, a kernel gets the next message from
 * the message queue and forwards it to the corresponding handlers. If the message queue
 * is empty the kernel goes to sleep and only wakes up when a new request arrives.
 */

#include "oe_defines.h"
#include "oe_request_map.h"
#include "oe_message_queue.h"
#include "oe_core_mod.h"

#if OE_USE_SYSTEM_REQUESTS
#include "oe_core_intf.h"
#endif

struct OE_Kernel_s
{
    /* The ID that this kernel has during operation. It is set by the core. */
    OE_KernelID_t KernelID;

    /* Connection to the core. */
    OE_Core_t *Core;

    /* The request map maps request IDs to message handlers. */
    OE_RequestMap_t RequestMap;
}; /* A typedef is done in oe_defines.h */

/**
 * @brief Initialize the kernel statically.
 *
 * This function initializes the request map
 * and connects the kernel to the core.
 *
 * @param Kernel The pointer to the kernel to be initialized.
 */
OE_Error_t OE_Kernel_staticInit(
    OE_Kernel_t *Kernel);

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
 * @return OE_Error_t An error is returned if
 * - registering the handlers results in an error.
 * Otherwise OE_ERROR_NONE is returned.
 */
OE_Error_t OE_Kernel_registerHandlers(
    OE_Kernel_t *Kernel,
    OE_RequestID_t *RequestIDs,
    OE_MessageHandler_t *RequestHandlers,
    size_t NumberOfRequests);

/**
 * @brief Unregister a list of message handlers for a list of request IDs.
 *
 * This function is the counterpart to the OE_Kernel_registerHandlers() function.
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
void OE_Kernel_unregisterHandlers(
    OE_Kernel_t *Kernel,
    OE_RequestID_t *RequestIDs,
    OE_MessageHandler_t *RequestHandlers,
    size_t NumberOfRequests);

/**
 * @brief Checks if at least one handler is registered for a given request ID.
 *
 * @param Kernel The pointer to the kernel whose request map is checked.
 *
 * @param RequestID The request ID to be checked.
 *
 * @return true At least one handler is registered.
 * @return false No handler is registered.
 */
bool OE_Kernel_handlerRegistered(
    OE_Kernel_t *Kernel,
    OE_RequestID_t RequestID);

/**
 * @brief The main routine of the kernel.
 *
 * In this function the kernel does its work. The function is an endless loop
 * consisting of three parts:
 * 1. The kernel tries to get a new message from the core.
 * 2. If there is a new message, the kernel processes the message.
 * 3. If there is no new message, the kernel goes into IDLE().
 *
 * New messages are sent to the core by ISRs or modules.
 *
 * @param Kernel The pointer to the kernel that runs.
 */
void OE_Kernel_run(
    OE_Kernel_t *Kernel);

/**
 * @brief Run a single cycle of the kernel main routine.
 *
 * The OE_Kernel_run() function is an endless loop consisting of three parts:
 * 1. The kernel tries to get a new message from the core.
 * 2. If there is a new message, the kernel processes the message.
 * 3. If there is no new message, the kernel goes into IDLE().
 *
 * OE_Kernel_runOnce() does basically the same thing but not in an endless loop.
 * Instead it only runs one cycle. This means that one message is fetched and
 * processed and then the function returns.
 *
 * This function may be used in software tests where a message is placed in the
 * message queue. The behaviour of the system can then be observed step by step.
 *
 * @param Kernel The pointer to the kernel that runs once.
 * 
 * @return true A message was handled.
 * @return false No message was handled.
 */
bool OE_Kernel_runOnce(
    OE_Kernel_t *Kernel);

/**
 * @brief Handle a new message.
 *
 * This method is called after a new message was received from the core.
 * According to the message type the message is forwarded as either a request or
 * a response.
 *
 * @param Kernel The pointer to the kernel that handles the message.
 *
 * @param Message Pointer to the received message.
 */
void OE_Kernel_handleMessage(
    OE_Kernel_t *Kernel,
    OE_Message_t *Message);

#endif // OE_KERNEL_H