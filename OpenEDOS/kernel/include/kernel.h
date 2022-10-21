/**
* OpenEDOS Kernel v1.2.0
* 
* Copyright (c) 2022 Samuel Ardaya-Lieb
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
 * This file is the class header of the kernel class. (An event kernel might also 
 * be referred to as a kernel.) The event kernel is the core class of the operating system.
 * The actual functionality of the application is implemented inside modules.
 * The kernel however manages these modules and their communication.
 *  
 * A kernel's job is to register modules and distribute messages to those modules. Messages 
 * are used to transport events, service requests and service responses. One kernel is in 
 * command of one message queue (which is a FIFO). In its main routine, a kernel takes the next 
 * message from the message queue and forwards it to the corresponding modules. When the 
 * message queue is empty the kernel goes to sleep and only wakes up when a new event occures.
 */

#include "defines.h"
#include "event_map.h"
#include "service_map.h"
#include "queue.h"
#include "module.h"

/* Make the kernel switch class known to the compiler */
class KernelSwitch_c;

class Kernel_c
{
public:
    /**
     * @brief Initialize the kernel.
     * 
     * This function initializes the event register, the event map and service map
     * and the message queue. It also sets several variables to initial values. 
     */
    void init(void);

    /**
     * @brief Connect a kernel to the kernel switch.
     * 
     * This function simply sets the pointer to the kernel switch.
     * 
     * @param KernelSwitch The pointer to the kernel switch.
     */
    void connect(
        KernelSwitch_c *KernelSwitch);

    /**
     * @brief Connect a module to the kernel.
     * 
     * The process of registering a module at the kernel starts with this function.
     * This function has to be called in order to make a specific module part of 
     * the OpenEDOS application.
     * 
     * @param Module Pointer to the module to be registered at the kernel.
     * 
     * @return Error_t An error is returned if
     * - there is no module space available.
     * - the function call inside this function returns an error.
     * Otherwise ERROR_NONE is returned. 
     */
    Error_t connectModule(
        Module_c *Module);

    /**
     * @brief Initialize all connected modules.
     * 
     * This function should be called after all modules are connected to the kernel.
     * The function simply calls the init() function of each connected module.
     * 
     * @return Error_t An error is returned if
     * - the initialization of one of the modules returns an error.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t initModules(void);

    /**
     * @brief Start the operation of the application.
     * 
     * Sometimes, specific actions have to be taken by a module at the very beginning 
     * of operation in an application, e.g. certain services have to be requested. 
     * This can be done inside the start() function of that module.
     * 
     * This function simply calls the start() method of all connected modules.
     */
    void start(void);

    /**
     * @brief Subscribe to a list of events.
     * 
     * This method is used by modules to subscribe to one or more events.
     * This can be done at any time during operation. Once an event is subscribed,
     * the kernel will update the module whenever that event occures. If an error
     * occures during this method, the module won't be subscribed to any of the
     * given event IDs.
     *      
     * @param EventIDs The pointer to an array of event IDs, provided by the
     * module that wants to subscribe to those events.
     * 
     * @param EventIDCount The number of event IDs in the array.
     * 
     * @param SubscriberID The module ID of the module that wants to subscribe.
     * 
     * @return Error_t An error is returned if
     * - the module ID is invalid.
     * - one of the event IDs is invalid.
     * - there is no space left in the event map for one of the event IDs.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t subscribeEvents(
        EventID_t *EventIDs, 
        size_t EventIDCount,
        ModuleID_t SubscriberID);

    /**
     * @brief Unsubscribe to a list of events.
     * 
     * This method is the counterpart to the subscribeEvents() method. 
     * It is used by a module to unsubscribe to one or more events. 
     * This can be done at any time during operation. Once an event is unsubscribed,
     * the kernel will no longer update the module when that event occures. If an error
     * occures during this method, the module won't be unsubscribed to any of the
     * given event IDs.
     * 
     * @param EventIDs The pointer to an array of event IDs, provided by the 
     * module that will no longer be subscribed to these events.
     * 
     * @param EventIDCount The number of event IDs in the array.
     * 
     * @param SubscriberID The module ID of the module that wants to unsubscribe.
     * 
     * @return Error_t An error is returned if
     * - the module ID is invalid.
     * - one of the event IDs is invalid.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t unsubscribeEvents(
        EventID_t *EventIDs, 
        size_t EventIDCount,
        ModuleID_t SubscriberID);

    /**
     * @brief Offer a list of services.
     * 
     * This method is used by modules to offer a list of services.
     * This can be done at any time during operation. Once a service is offered,
     * the kernel will update the module whenever that service is requested. Note 
     * that according to the single responsibility principle, only one module can 
     * offer a specific service at a time. If an error occures during this method, 
     * the module won't offer any of the given services.
     * 
     * @param ServiceIDs The pointer to an array of service IDs, provided by
     * the module that wants to offer those services.
     * 
     * @param ServiceIDCount The number of service IDs in the array.
     * 
     * @param ProviderAddress The pointer to the address of the module that 
     * wants to offer the services.
     * 
     * @return Error_t An error is returned if
     * - the module address is invalid.
     * - one of the service IDs is invalid.
     * - another module is already registered for one of the service IDs.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t offerServices(
        ServiceID_t *ServiceIDs, 
        size_t ServiceIDCount,
        ModuleAddress_t *ProviderAddress);

    /**
     * @brief Withdraw a list of services.
     * 
     * This method is used by modules if they no longer want to offer one
     * or more services. This can be done at any time during operation. Once 
     * a service is withdrawn, the kernel will no longer update the module if
     * that service is requested. If an error occures during this method, the 
     * module won't withdraw any of the given services.
     * 
     * @param ServiceIDs The pointer to an array of service IDs, provided by
     * the module that wants to withdraw those services.
     * 
     * @param ServiceIDCount The number of service IDs in the array.
     * 
     * @param ProviderAddress The pointer to the address of the module that 
     * wants to withdraw the services.
     * 
     * @return Error_t An error is returned if
     * - the module address is invalid.
     * - one of the service IDs is invalid.
     * - another module is registered for one of the service IDs.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t withdrawServices(
        ServiceID_t *ServiceIDs, 
        size_t ServiceIDCount,
        ModuleAddress_t *ProviderAddress);

    /**
     * @brief The main routine of the kernel.
     * 
     * In this function the kernel starts working. The function is an endless loop
     * consisting of three parts:
     * 1. The kernel tries to get a new message from the kernel switch.
     * 2. If there is a new message, the kernel processes the message.
     * 3. If there is no new message, the kernel goes into IDLE().
     * 
     * New messages are sent to the kernel switch by ISRs or modules. 
     */
    void run(void);

    /**
     * @brief Run a single cycle of the kernel main routine.
     * 
     * The run() function is an endless loop consisting of three parts:
     * 1. The kernel tries to get a new message from the kernel switch.
     * 2. If there is a new message, the kernel processes the message.
     * 3. If there is no new message, the kernel goes into IDLE().
     * 
     * runOnce() does basically the same thing but not in an endless loop.
     * Instead it only runs one cycle. This means that one message gets popped and
     * processed and then the function returns.
     * 
     * This method can be used in software tests where a message is placed in the 
     * message queue. The behaviour of the system can then be observed step by step
     * using this method. 
     */
    void runOnce(void);

    /**
     * @brief Set the ID of a kernel.
     * 
     * @param KernelID The ID that this kernel will have during operation.
     */
    void setID(
        KernelID_t KernelID);

    /**
     * @brief Get the ID of a kernel.
     * 
     * @return KernelID_t The ID of the kernel.
     */
    KernelID_t getID(void);

private:
    /**
     * @brief Handle a new message.
     * 
     * This method is called after a new message was received from the kernel switch.
     * According to the message type the message is forwarded as either an event, a 
     * request or a response. 
     * 
     * @param Message Pointer to the received message.
     */
    void handleMessage(
        Message_t *Message);
    
    /* The ID that this kernel has during operation. It is set by the kernel switch. */
    KernelID_t KernelID;

    /* Connection to the kernel switch. */
    KernelSwitch_c *KernelSwitch;
        
    /* Pointers to all connected modules are stored in an array */
    Module_c *Modules[NUMBER_OF_MODULES];
    /* Keep track of how many modules are connected */
    ModuleCount_t ModuleCount;
    
    /* The service map and event map are used to save module information */
    Identifier_t EventMapMemory[NUMBER_OF_EVENTS*NUMBER_OF_MODULES];
    MapNode_t EventMapNodes[NUMBER_OF_EVENTS];
    EventMap_c EventMap;
};

#endif//EVENT_KERNEL_H