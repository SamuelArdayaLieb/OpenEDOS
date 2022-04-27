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

#include "include/kernel.h"
#include <string.h>

void Kernel_c::init(void)
{
    size_t Count;

    /* init event register */
    for(Count = 0; Count < NUMBER_OF_EVENTS; Count++)
    {
        this->EventRegister[Count].CurrentEventCount = 0;
        this->EventRegister[Count].MaxEventCount = MAX_EVENT_COUNT;
    }

    /* init event queue */
    this->EventQueue.init((void*)&this->EventQueueMemory[0],                    
                          (void*)&this->EventQueueMemory[EVENT_QUEUE_LENGTH-1], 
                          EVENT_QUEUE_LENGTH,                                   
                          sizeof(Event_t));                                     

    /* Init message queue */
    this->MessageQueue.init((void*)&this->MessageQueueMemory[0], 
                            (void*)&this->MessageQueueMemory[MESSAGE_QUEUE_LENGTH-1],
                            MESSAGE_QUEUE_LENGTH, 
                            sizeof(Message_t));

    /* Init the module storage */
    for(Count = 0; Count < NUMBER_OF_MODULES; Count++)
    {
        this->Modules[Count] = nullptr;
    }

    this->ModuleCount = 0;

    /* Init the event map and the service map */
    this->EventMap.init();
    this->ServiceMap.init();
}

Error_t Kernel_c::connectModule(Module_c *Module)
{
    return Module->connect(this);
}

Error_t Kernel_c::loadModule(Module_c *Module, ModuleRegistration_t *ModuleRegistration)
{
    /* Check if another module can connect */
    if(this->ModuleCount >= NUMBER_OF_MODULES)
    {
        return ERROR_NO_MODULE_SPACE_AVAILABLE;
    }
    
    ModuleID_t NewModuleID = this->ModuleCount;
    Error_t Error;

    /** 
     * The information passed by the new module is permanently stored in the event map 
     * and service map.
     */
    Error = this->EventMap.registerModule(ModuleRegistration->EventIDs, 
                                          ModuleRegistration->EventIDCount, NewModuleID);

    /* Did registering the module in the event map go wrong? */
    if(Error != ERROR_NONE)
    {
        /* Abort the registration */
        return Error;
    }
    
    Error = this->ServiceMap.registerModule(ModuleRegistration->ServiceIDs, 
                                            ModuleRegistration->ServiceIDCount, NewModuleID);

    /* Did registering the module in the service map go wrong? */                                       
    if(Error != ERROR_NONE)
    {
        /** 
         * Abort the registration. This means the module has to be unregistered from
         * the event map.
         */
        this->EventMap.unregisterModule(ModuleRegistration->EventIDs, 
                                        ModuleRegistration->EventIDCount, NewModuleID);

        return Error;  
    }

    /**
     * Both the event map and the service map accepted the module registration.
     * This means that the new module gets a module ID and can connect to the kernel. 
     */
    Module->setID(NewModuleID);

    this->Modules[NewModuleID] = Module;

    this->ModuleCount++;

    return ERROR_NONE;    
}

Error_t Kernel_c::initModules(void)
{
    Error_t Error;
    
    for(ModuleID_t ModuleID = 0; ModuleID < this->ModuleCount; ModuleID++)
    {
        Error = this->Modules[ModuleID]->init();

        if(Error != ERROR_NONE)
        {
            return Error;
        }
    }

    return ERROR_NONE;
}

Message_c Kernel_c::newMessage(void)
{
    /* Allocate message memory in the message queue */
    Message_t *MessageMemory = (Message_t*)this->MessageQueue.allocateItem();

    /* Create an object of the message class */
    Message_c Message;
    /* Assign the message memory to the object */
    Message.init(MessageMemory);

    return Message;    
}

Error_t Kernel_c::setEventEntry(EventID_t EventID)
{
    /* Check if the event ID is valid */
    if(EventID >= NUMBER_OF_EVENTS)
    {
        return ERROR_EVENT_ID_INVALID;
    }
    
    /* This makes accessing the memory a little bit easier */
    EventRegisterEntry_t *EventEntry = &this->EventRegister[EventID];

    /* Check if the register is full */
    if(EventEntry->CurrentEventCount >= EventEntry->MaxEventCount)
    {
        return ERROR_EVENT_REGISTER_FULL;
    }

    /* Increase the current event count */
    EventEntry->CurrentEventCount++;

    return ERROR_NONE; 
}

Error_t Kernel_c::clearEventEntry(EventID_t EventID)
{
    /* Check if the event ID is valid */
    if(EventID >= NUMBER_OF_EVENTS)
    {
        return ERROR_EVENT_ID_INVALID;
    }

    /* Check if the event register actually holds an event */
    if(this->EventRegister[EventID].CurrentEventCount == 0)
    {
        return ERROR_EVENT_REGISTER_EMPTY;
    }

    /* Decrease the current event count */
    this->EventRegister[EventID].CurrentEventCount--;

    return ERROR_NONE;
}

Error_t Kernel_c::pushEvent(Event_t *Event, bool InterruptableContext)
{
    /**
     * Accessing the event register and event queue is not interrupt safe.
     * Hence interrupts need to be disabled if we are in an interruptable context.
     */ 
    if(InterruptableContext)
    {
        DISABLE_INTERRUPTS();
    }

    Error_t Error;

    /* Only queue the event if it has not reached its limit in the event register */
    Error = this->setEventEntry(Event->EventID);

    if(Error == ERROR_NONE)
    {
        /* Can the event be queued? */
        if(this->EventQueue.pushItem(Event) != true)
        {
            Error = ERROR_EVENT_QUEUE_FULL;

            this->clearEventEntry(Event->EventID);
        }
    }

    /* Only enable interrupts if they have been disabled before */
    if(InterruptableContext)
    {
        ENABLE_INTERRUPTS();
    }

    return Error;   
}

bool Kernel_c::popEvent(Event_t *Event)
{
    bool EventPopped = false;

    /**
     * Accessing the event register and event queue is not interrupt safe.
     * Hence interrupts need to be disabled. As this function is only called
     * inside the kernel main routine the context is always interruptable.
     */
    DISABLE_INTERRUPTS();

    /* Get the next event from the event queue (if there is one) */
    if(this->EventQueue.popItem(Event) == true)
    {
        EventPopped = true;

        /* Clear the entry in the event register */
        this->clearEventEntry(Event->EventID);   
    }

    ENABLE_INTERRUPTS();

    return EventPopped;
}

Error_t Kernel_c::forwardEvent(Event_t *Event)
{
    ModuleID_t ModuleID;
    EventMapNode_t *EventMapNode;

    /* We need a list of all modules listening to this event */
    Error_t Error = this->EventMap.getModuleList(Event->EventID, &EventMapNode);

    /* Getting the module list might result in an error */
    if(Error != ERROR_NONE)
    {
        return Error;
    }

    /* Distribute the event to all modules listed in the event map node */
    for(ModuleCount_t Count = 0; Count < EventMapNode->ModuleCount; Count++)
    {
        ModuleID = EventMapNode->ModuleIDs[Count];

        this->Modules[ModuleID]->update(Event);
    }

    return ERROR_NONE;   
}

Error_t Kernel_c::forwardMessage(Message_t *Message)
{
    ModuleID_t ModuleID;

    /* Does the message already have a destination? */
    if(Message->MessageHeader.DestinationID == NO_DESTINATION)
    {
        Error_t Error = this->ServiceMap.getModuleID(Message->MessageHeader.ServiceID, 
                                                     &ModuleID);
        
        /* Getting the module ID might result in an error */
        if(Error != ERROR_NONE)
        {
            return Error;
        }
    }
    else
    {   
        /* In some cases the message already has a destination */
        ModuleID = Message->MessageHeader.DestinationID;
    }

    /**
     * Until this point the message was present in form of a struct.
     * The modules however expect a pointer to an object of the message class.
     */
    Message_c MessageObject;

    /* Give the message object access to the message memory */
    MessageObject.init(Message);

    /* Forward the message */
    this->Modules[ModuleID]->update(&MessageObject);

    return ERROR_NONE;  
}

Error_t Kernel_c::processEvent(Event_t *Event)
{
    /* Forward the event to all listening modules */
    Error_t Error = this->forwardEvent(Event);

    /* Forwarding the event might result in an error */
    if(Error != ERROR_NONE)
    {
        return Error;
    }

    Message_t Message;

    /* Distribute messages from the message queue to the modules */
    while(this->MessageQueue.popItem(&Message))
    {
        Error = this->forwardMessage(&Message);

        /* Forwarding the message might result in an error */
        if(Error != ERROR_NONE)
        {
            return Error;
        }
    }

    return ERROR_NONE;
}

void Kernel_c::run(void)
{
    Event_t Event;

    while(true)
    {    
        if(this->popEvent(&Event))
        {
            this->processEvent(&Event);
        }
        else
        {
            SLEEP();
        }
    }   
}
