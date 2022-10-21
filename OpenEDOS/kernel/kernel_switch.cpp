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

#include "include/kernel_switch.h"
#include <string.h>

void KernelSwitch_c::init(void)
{
    size_t Count;
    KernelID_t KernelID;

    for(KernelID = 0; KernelID < NUMBER_OF_KERNELS; KernelID++)
    {
        /* init event register */
        for(Count = 0; Count < NUMBER_OF_EVENTS; Count++)
        {
            this->EventRegisters[KernelID][Count].CurrentEventCount = 0;
            this->EventRegisters[KernelID][Count].MaxEventCount = MAX_EVENT_COUNT;
        }

        /* Init message queue */
        this->MessageQueues[KernelID].init(
            (void*)&this->MessageQueueMemory[KernelID][0], 
            (void*)&this->MessageQueueMemory[KernelID][MESSAGE_QUEUE_LENGTH-1],
            MESSAGE_QUEUE_LENGTH, 
            sizeof(Message_t));

        this->Kernels[KernelID] = nullptr;            
    }

    this->KernelCount = 0;

    this->EventMap.init(
        this->EventMapMemory,
        this->EventMapNodes,
        NUMBER_OF_KERNELS);
        
    this->ServiceMap.init();            
}

Error_t KernelSwitch_c::connectKernel(
    Kernel_c *Kernel)
{
    if(this->KernelCount >= NUMBER_OF_KERNELS)
    {
        return ERROR_NO_MEMORY_AVAILABLE;
    }

    Kernel->setID(
        this->KernelCount);

    this->Kernels[this->KernelCount] = Kernel;

    this->KernelCount++;

    Kernel->connect(
        this);

    return ERROR_NONE;
}

Error_t KernelSwitch_c::subscribeEvents(
    EventID_t *EventIDs, 
    size_t EventIDCount,
    KernelID_t SubscriberID)
{
    if(SubscriberID >= NUMBER_OF_KERNELS)
    {
        return ERROR_KERNEL_ID_INVALID;
    }
        
    ENTER_CRITICAL();

    Error_t Error = this->EventMap.registerID(
        EventIDs,
        EventIDCount,
        SubscriberID);

    EXIT_CRITICAL();

    return Error;
}

Error_t KernelSwitch_c::unsubscribeEvents(
    EventID_t *EventIDs, 
    size_t EventIDCount,
    KernelID_t SubscriberID)
{
    if(SubscriberID >= NUMBER_OF_KERNELS)
    {
        return ERROR_KERNEL_ID_INVALID;
    }
        
    ENTER_CRITICAL();

    Error_t Error = this->EventMap.unregisterID(
        EventIDs,
        EventIDCount,
        SubscriberID);

    EXIT_CRITICAL();

    return Error;
}

Error_t KernelSwitch_c::offerService(
    ServiceID_t *ServiceIDs, 
    size_t ServiceIDCount,
    ModuleAddress_t *ProviderAddress)
{        
    ENTER_CRITICAL();

    Error_t Error = this->ServiceMap.registerServiceProvider(
        ServiceIDs,
        ServiceIDCount,
        ProviderAddress);

    EXIT_CRITICAL();

    return Error;
}

Error_t KernelSwitch_c::withdrawService(
    ServiceID_t *ServiceIDs, 
    size_t ServiceIDCount,
    ModuleAddress_t *ProviderAddress)
{    
    ENTER_CRITICAL();

    Error_t Error = this->ServiceMap.unregisterServiceProvider(
        ServiceIDs,
        ServiceIDCount,
        ProviderAddress);

    EXIT_CRITICAL();

    return Error;
}

Error_t KernelSwitch_c::sendMessage(
    Message_t *Message)
{
    Error_t Error;
    uint8_t MessageType = Message->MessageHeader.MessageInformation & MESSAGE_TYPE_MASK;

    switch(MessageType)
    {
        case MESSAGE_TYPE_EVENT:
        {
            MapNode_t *MapNode;
            KernelID_t KernelID;
            EventID_t EventID = Message->MessageHeader.ContentID;
            
            ENTER_CRITICAL();

            this->EventMap.getIDs(
                EventID,
                &MapNode);

            for(size_t Count = 0; Count < MapNode->IDCount; Count++)
            {
                KernelID = MapNode->IDs[Count];
                
                Error = this->setEventEntry(
                    KernelID, 
                    EventID);

                Message->MessageHeader.DestinationAddress.KernelID = KernelID;

                if(Error == ERROR_NONE)
                {
                    if(this->MessageQueues[KernelID].pushItem(Message) != true)
                    {
                        this->clearEventEntry(
                            KernelID,
                            EventID);
                    }

                    RESUME(KernelID);
                }
            }

            EXIT_CRITICAL();

            return ERROR_NONE;
        } break;

        case MESSAGE_TYPE_REQUEST:
        {
            ModuleAddress_t ProviderAddress;
            ServiceID_t ServiceID = Message->MessageHeader.ContentID;
            
            ENTER_CRITICAL();

            this->ServiceMap.getServiceProvider(
                ServiceID, 
                &ProviderAddress);
            
            Message->MessageHeader.DestinationAddress = ProviderAddress;

            if(this->MessageQueues[ProviderAddress.KernelID].pushItem(Message) != true)
            {
                EXIT_CRITICAL();
                
                return ERROR_MESSAGE_QUEUE_FULL;
            }

            RESUME(ProviderAddress.KernelID);

            EXIT_CRITICAL();

            return ERROR_NONE;
        } break;

        case MESSAGE_TYPE_RESPONSE:
        {
            KernelID_t KernelID = Message->MessageHeader.DestinationAddress.KernelID;
            
            ENTER_CRITICAL();

            if(this->MessageQueues[KernelID].pushItem(Message) != true)
            {
                EXIT_CRITICAL();
                
                return ERROR_MESSAGE_QUEUE_FULL;
            }

            RESUME(KernelID);

            EXIT_CRITICAL();

            return ERROR_NONE;
        } break;

        default:
        {
            NOP();
        }
    }

    return ERROR_NONE;
}

bool KernelSwitch_c::getMessage(
    KernelID_t KernelID,
    Message_t *Message)
{
    bool ReturnValue = false;
    
    ENTER_CRITICAL();

    if(this->MessageQueues[KernelID].popItem(Message))
    {
        /* If the message contains an event, we need to clear the event entry. */
        if((Message->MessageHeader.MessageInformation & MESSAGE_TYPE_MASK) == MESSAGE_TYPE_EVENT)
        {
            this->clearEventEntry(
                KernelID,
                Message->MessageHeader.ContentID);
        }

        ReturnValue = true;
    }

    EXIT_CRITICAL();

    return ReturnValue;
}

Error_t KernelSwitch_c::setEventEntry(
    KernelID_t KernelID,
    EventID_t EventID)
{
    if(KernelID >= NUMBER_OF_KERNELS)
    {
        return ERROR_KERNEL_ID_INVALID;
    }
    
    /* Check if the event ID is valid */
    if(EventID >= NUMBER_OF_EVENTS)
    {
        return ERROR_EVENT_ID_INVALID;
    }
    
    /* This makes accessing the memory a little bit easier */
    EventRegisterEntry_t *EventEntry = &this->EventRegisters[KernelID][EventID];

    /* Check if the register is full */
    if(EventEntry->CurrentEventCount >= EventEntry->MaxEventCount)
    {   
        return ERROR_EVENT_REGISTER_FULL;
    }

    /* Increase the current event count */
    EventEntry->CurrentEventCount++;

    return ERROR_NONE; 
}

Error_t KernelSwitch_c::clearEventEntry(
    KernelID_t KernelID,
    EventID_t EventID)
{
    /* Check if the event ID is valid */
    if(EventID >= NUMBER_OF_EVENTS)
    {
        return ERROR_EVENT_ID_INVALID;
    }

    /* Check if the event register actually holds an event */
    if(this->EventRegisters[KernelID][EventID].CurrentEventCount > 0)
    {   
        /* Decrease the current event count */
        this->EventRegisters[KernelID][EventID].CurrentEventCount--;
    }

    return ERROR_NONE;
}