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

#ifndef KERNEL_SWITCH_H
#define KERNEL_SWITCH_H

#include "defines.h"
#include "kernel.h"
#include "event_map.h"
#include "service_map.h"
#include "queue.h"

class KernelSwitch_c 
{
public:
    /**
     * @brief Initialize the kernel switch.
     * 
     * This function initializes the event registers, the event map and service map
     * and the message queues. It also sets several variables to initial values. 
     */
    void init(void);

    /**
     * @brief Connect a kernel to the kernel switch.
     *      
     * @param Module Pointer to the kernel to be registered at the kernel switch.
     * 
     * @return Error_t An error is returned if
     * - the function call inside this function returns an error.
     * Otherwise ERROR_NONE is returned. 
     */
    Error_t connectKernel(
        Kernel_c *Kernel);

    /**
     * @brief Subscribe to a list of events.
     * 
     * This method is used by kernels to subscribe to one or more events.
     * This can be done at any time during operation. Once an event is subscribed,
     * the kernel switch will update the kernel whenever that event occures. If an error
     * occures during this method, the kernel won't be subscribed to any of the
     * given event IDs.
     * 
     * @param EventIDs The pointer to an array of event IDs, provided by the
     * kernel that wants to subscribe to those events.
     * 
     * @param EventIDCount The number of event IDs in the array.
     * 
     * @param SubscriberID The ID of the kernel that wants to subscribe to
     * the list of events.
     * 
     * @return Error_t An error is returned if
     * - the subscriber ID is invalid.
     * - one of the event IDs is invalid.
     * - there is no space left in the event map for one of the event IDs.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t subscribeEvents(
        EventID_t *EventIDs, 
        size_t EventIDCount,
        KernelID_t SubscriberID);

    /**
     * @brief Unsubscribe to a list of events.
     * 
     * This method is the counterpart to the subscribeEvents() method. 
     * It is used by a kernel to unsubscribe to one or more events. 
     * This can be done at any time during operation. Once an event is unsubscribed,
     * the kernel switch will no longer update the kernel when that event occures. 
     * If an error occures during this method, the kernel won't be unsubscribed to 
     * any of the given event IDs.
     * 
     * @param EventIDs The pointer to an array of event IDs, provided by the 
     * module that will no longer be subscribed to these events.
     * 
     * @param EventIDCount The number of event IDs in the array.
     * 
     * @param SubscriberID The ID of the kernel that wants to unsubscribe to
     * the list of events.
     * 
     * @return Error_t An error is returned if
     * - the subscriber ID is invalid.
     * - one of the event IDs is invalid.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t unsubscribeEvents(
        EventID_t *EventIDs, 
        size_t EventIDCount,
        KernelID_t SubscriberID);

    /**
     * @brief Offer a list of services.
     * 
     * This method is used by kernels to offer a list of services.
     * This can be done at any time during operation. Once a service is offered,
     * the kernel switch will update the provider whenever that service is requested. 
     * Note that according to the single responsibility principle, only one address can 
     * offer a specific service at a time. If an error occures during this method, 
     * the provider won't offer any of the given services.
     *       
     * @param ServiceIDs The pointer to an array of service IDs that the service 
     * provider wants to offer.
     * 
     * @param ServiceIDCount The number of service IDs in the array.
     * 
     * @param ProviderAddress The address of the service provider.
     * 
     * @return Error_t An error is returned if
     * - the provider address is invalid.
     * - one of the service IDs is invalid.
     * - another address is already registered for one of the service IDs.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t offerService(
        ServiceID_t *ServiceIDs, 
        size_t ServiceIDCount,
        ModuleAddress_t *ProviderAddress);

    /**
     * @brief Withdraw a list of services.
     * 
     * This method is used by kernels if they no longer want to offer one
     * or more services. This can be done at any time during operation. Once 
     * a service is withdrawn, the kernel switch will no longer update the kernel 
     * if that service is requested. If an error occures during this method, the 
     * kernel won't withdraw any of the given services.
     * 
     * @param ServiceIDs The pointer to an array of service IDs that the service 
     * provider wants to withdraw.
     * 
     * @param ServiceIDCount The number of service IDs in the array.
     * 
     * @param ProviderAddress The address of the service provider.
     * 
     * @return Error_t An error is returned if
     * - the provider address is invalid.
     * - one of the service IDs is invalid.
     * - another address is registered for one of the service IDs.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t withdrawService(
        ServiceID_t *ServiceIDs, 
        size_t ServiceIDCount,
        ModuleAddress_t *ProviderAddress);

    /**
     * @brief Send a message to the kernel switch.
     * 
     * This is the interface method that is used to send messages to the 
     * kernel switch. The kernel switch checks some of the variables in the 
     * message header before adding the new message to the message queue.
     * 
     * @param Message The pointer to the message 
     * 
     * @return Error_t An error is returned if
     * - one of the variables in the message header is invalid.
     * - queueing the message fails.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t sendMessage(
        Message_t *Message);

    /**
     * @brief Get a new message from the message queue.
     * 
     * This method is called inside the kernel main routine.
     * It is used to get the next message from the message queue.
     * 
     * @param KernelID The ID of the kernel that calls the function.
     * 
     * @param Message The pointer to a message variable. The next
     * message is copied to the pointer.
     * 
     * @return true if a message was taken from the message queue. 
     * 
     * @return false if no message was taken from the message queue.
     */
    bool getMessage(
        KernelID_t KernelID,
        Message_t *Message);

private:
    /**
     * @brief Set an entry in the event register.
     * 
     * When a new event occures it has to be registered in the event register before
     * it can be queued. This is done by increasing the current event count for the
     * given event ID. Further processing of an event only takes place if the event
     * register is not full.
     * 
     * @param KernelID The ID of the kernel whose event register is used.
     * 
     * @param EventID The ID of the new event that has to be registered.
     * 
     * @return Error_t An error is returned if
     * - the kernel ID is invalid.
     * - the event ID is invalid.
     * - the event register is full.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t setEventEntry(
        KernelID_t KernelID,
        EventID_t EventID);

    /**
     * @brief Clear an entry in the event register.
     * 
     * This function is basically the opposite of setEventEntry(). It is called when
     * an event message leaves the message queue.
     * 
     * @param KernelID The ID of the kernel whose event register is used.
     * 
     * @param EventID ID of the event.
     */
    Error_t clearEventEntry(
        KernelID_t KernelID,
        EventID_t EventID);
    
    /* Kernel connections */
    Kernel_c *Kernels[NUMBER_OF_KERNELS];
    KernelCount_t KernelCount;

    /* Event map */
    Identifier_t EventMapMemory[NUMBER_OF_EVENTS*NUMBER_OF_KERNELS];
    MapNode_t EventMapNodes[NUMBER_OF_EVENTS];
    EventMap_c EventMap;

    /* Service map */
    ServiceMap_c ServiceMap;

    /**
     * The event register serves to keep track of how many events are queued at a time.
     * There is one register entry for each event ID.
     */
    EventRegisterEntry_t EventRegisters[NUMBER_OF_KERNELS][NUMBER_OF_EVENTS];

    /* Message queues */
    Message_t MessageQueueMemory[NUMBER_OF_KERNELS][MESSAGE_QUEUE_LENGTH];
    Queue_c MessageQueues[NUMBER_OF_KERNELS];
};

#endif//KERNEL_SWITCH_H