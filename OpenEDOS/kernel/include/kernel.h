/**
* OpenEDOS Kernel v1.0.0
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
 * This file is the class header of the kernel class. The kernel is the core class
 * of the operating system. Its job is to register modules and distribute events
 * and messages to those modules. The kernel is in command of two queues, the event
 * queue and the message queue. In its main routine, the kernel distributes new 
 * events to the system and then forwards all messages following the occured event
 * to the corresponding modules. After the event queue is emptied the kernel goes to 
 * sleep and only wakes up when a new event occures.
 */

/**
 * Needed for various defines and type definitions.
 * The OpenEDOSConfig, event enum, service enum and port are also included by this. 
 */
#include "defines.h"

/**
 * The actual functionality of the application is implemented inside modules.
 * The kernel however manages these modules and their communication.
 */
#include "module.h"

/* The kernel registers modules via one event map and one service map */
#include "event_map.h"
#include "service_map.h"

/* Messages are needed by the kernel for providing an interface for modules */
#include "message.h"

/* The kernel controls two queues: the event queue and the message queue */
#include "queue.h"


class Kernel_c
{
public:
    /**
     * @brief Initialize the kernel.
     * 
     * This function initializes the event register, the event map and service map
     * and the event queue and message queue. It also sets several variables to
     * initial values. 
     */
    void init(void);

    /**
     * @brief Connect a module to the kernel.
     * 
     * The process of registrating a module at the kernel starts with this function.
     * It is merely a wrapper for the connect() function of the module. It's necessary 
     * though because all module functions are private and, as a friend, only the 
     * kernel has access to that function. This function has to be called in order
     * to make a specific module part of the OpenEDOS application.
     * 
     * @param Module Pointer to the module that will be registered at the kernel.
     * 
     * @return Error_t An error is returned if
     * - the function call inside this function returns an error.
     * Otherwise ERROR_NONE is returned. 
     */
    Error_t connectModule(Module_c *Module);

    /**
     * @brief Load the module registration of a module into the kernel.
     * 
     * This function is part of registrating a module. It is called by a module inside
     * its connect() function. The module is supposed to provide a pointer to itself 
     * and a module  registration. The module registration contains the IDs of the events 
     * that the module wants to listen to and the IDs of the services that the module 
     * wants to offer. The kernel saves this information using the event map and service 
     * map. This function also sets the ID of the connected module.
     * 
     * @param Module Pointer to the module that wants to load its information. Usually
     * it's the this pointer of the module calling this function.
     * 
     * @param ModuleRegistration Pointer to a module registration provided by the module
     * calling this function.
     * 
     * @return Error_t An error is returned if
     * - the module count is at its maximum.
     * - registering the module in the event map fails.
     * - registering the module in the service map fails.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t loadModule(Module_c *Module, ModuleRegistration_t *ModuleRegistration);

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
     * @brief Create a new message in the message queue of the kernel. 
     * 
     * This function is the interface modules can use to communicate with the system.
     * When called, the function allocates space in the message queue and creates an
     * object of the message class. This object is set to point to the allocated
     * memory and is returned by the function. The caller can then use the received 
     * object to store information inside the message. Note that after calling this 
     * function the corresponding message struct is already queued. If the message
     * queue is full, a message object is returned nevertheless. But in that case the
     * message memory that the object points to is a nullptr. Hence this should always
     * be checked before trying to add data to the message.
     * 
     * @return Message_c Object of the message class containing a pointer to the 
     * message memory of the queueu. This pointer is a nullptr if the queue is full.
     */
    Message_c newMessage(void);

    /**
     * @brief Push an event to the event queue of the kernel.
     * 
     * Events can be generated by ISRs or modules. If that happens, the event has
     * to be pushed to the kernel in order to be processed. Pushing an event 
     * involves two different mechanisms:
     * 
     * 1. An entry has to be set in the event register. This is done to keep track
     * of how many events of each type are waiting to be processed at a time. There
     * is a maximum amount of events that can be entered in the event register. This 
     * is done in order to protect the application from a flooding of events from
     * any external source. While the event count for a specific event ID has reached 
     * its maximum in the event register, new events of that type are ignored.
     * 
     * 2. If the event register accepts the new event, the event (consisting of the
     * event ID and optional event parameters) is placed in the event queue. Events
     * placed in the queue will be processed in fifo order.
     * 
     * Accessing the event register and event queue is not interrupt safe. Hence 
     * interrupts have to be disabled if the context in which the function is called
     * is interruptable. This is usually true. A case where this is false is for example
     * an ISR with nested interrupts disabled.  
     * 
     * @param Event Pointer to the event struct that the caller wants to push to
     * the event queue.
     * 
     * @param InterruptableContext A flag that tells the kernel wether to disable
     * interrupts or not. If the current context of the caller is interruptable 
     * (which is usually the case) this MUST be set to true. Only if the context is
     * not interruptable this can be set to false. Note that setting this to true
     * in a context that is not or should not be interruptable can cause problems.
     * 
     * @return Error_t An error is returned if
     * - the event register is full
     * - the event queue is full
     * Otherwise ERROR_NONE is returned.
     */
    Error_t pushEvent(Event_t *Event, bool InterruptableContext);

    /**
     * @brief The main routine of the kernel.
     * 
     * In this function the kernel starts working. The function is an endless loop
     * consisting of three parts:
     * 1. The kernel tries to pop a new event from the event queue.
     * 2. If the event queue was not empty, the kernel processes the event.
     * 3. If the event queue was empty, the kernel goes to sleep.
     * 
     * New events are pushed to the event queue by ISRs or modules. ISRs also have 
     * to wake up the kernel. This way the kernel will sleep until a new event is 
     * generated by an ISR.
     */
    void run(void);

private:
    /**
     * @brief Set an entry in the event register.
     * 
     * When a new event occures it has to be registered in the event register before
     * it can be queued. This is done by increasing the current event count for the
     * given event ID. Further processing of an event only takes place if the event
     * register is not full.
     * 
     * @param EventID The ID of the new event that has to be registered.
     * 
     * @return Error_t An error is returned if
     * - the event ID is invalid.
     * - the event register is full.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t setEventEntry(EventID_t EventID);

    /**
     * @brief Clear an entry in the event register.
     * 
     * This function is basically the opposite of setEventEntry(). It is called when
     * the processing of an event is finished.
     * 
     * @param EventID ID of the event that has been completely processed.
     * 
     * @return Error_t An error is returned if
     * - the event ID is invalid.
     * - the event register is empty.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t clearEventEntry(EventID_t EventID);

    /**
     * @brief Pop an event from the event queue.
     * 
     * Events are queued by ISRs and modules. The kernel receives those events in
     * its main routine by popping them from the event queue. This is done using this 
     * function.
     * 
     * @param Event Pointer to store the popped event.
     *  
     * @return true Returned if a new event was popped from the queue. 
     * @return false Returned if a new event was not popped from the queue.
     */
    bool popEvent(Event_t *Event);

    /**
     * @brief Forward an event to listening modules.
     * 
     * In order to forward an event the kernel gets the list of module IDs from 
     * the event map. Then all listed modules are updated.
     * 
     * @param Event Pointer to the occured event.
     * 
     * @return Error_t An error is returned if
     * - getting the module list from the event map results in an error.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t forwardEvent(Event_t *Event);

    /**
     * @brief Forward a message to the corresponding module.
     * 
     * The destination of a message is determined by the kernel using the service ID
     * given in the message. The kernel gets the destination ID from the service map.
     * In case the message is a RETURN message the destination ID is already set.
     * In both cases the kernel will update the destination module. Note that the
     * message popped from the message queue is merely a struct containing the message
     * information. Updating a module however is done py providing an object of the
     * message class. That object is created in this function. 
     * 
     * @param Message Pointer to the message memory.
     * 
     * @return Error_t An error is returned if
     * - getting the module ID from the service map results in an error.
     * Otherwise ERROR_NONE is returned. 
     */
    Error_t forwardMessage(Message_t *Message);    

    /**
     * @brief Process a new event.
     * 
     * This function is part of the kernel main routine. Processing an event is done
     * in two different steps:
     * 
     * 1. The event is forwarded to all modules listening to this event. This may cause
     * modules to generate new events or send messages to the kernel.
     * 
     * 2. Messages are forwarded to the corresponding modules until the message queue is
     * empty.
     * 
     * @param Event Pointer to the event that will be processed.
     * 
     * @return Error_t An error is returned if
     * - forwarding the event returns an error.
     * - forwarding a message returns in an error.
     * Otherwise ERROR_NONE is returned. 
     */
    Error_t processEvent(Event_t *Event);

    /**
     * The event register serves to keep track of how many events are queued at a time.
     * There is one register entry for each event ID.
     * 
     * The event queue stores events if the event register accepts the event.
     * Memory for the event queue has to be provided.
     */
    EventRegisterEntry_t EventRegister[NUMBER_OF_EVENTS];
    Event_t EventQueueMemory[EVENT_QUEUE_LENGTH];
    Queue_c EventQueue;

    /* A queue to store messages. Memory has to be provided */
    Message_t MessageQueueMemory[MESSAGE_QUEUE_LENGTH];
    Queue_c MessageQueue;    
    
    /* Pointers to all connected modules are stored in an array */
    Module_c *Modules[NUMBER_OF_MODULES];
    /* Keep track of how many modules are connected */
    ModuleCount_t ModuleCount;
    
    /* The service map and event map are used to save module information */
    EventMap_c EventMap;
    ServiceMap_c ServiceMap;
};

#endif