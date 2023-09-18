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

#include "include/kernel.h"
#include "include/kernel_switch.h"
#include <string.h>

void Kernel_c::init(void)
{
    /* Init the module storage. */
    for(ModuleID_t ModuleID = 0; ModuleID < NUMBER_OF_MODULES; ModuleID++)
    {
        this->Modules[ModuleID] = nullptr;
    }

    this->ModuleCount = 0;

    /* Init the event map. */
    this->EventMap.init(
        this->EventMapMemory,
        this->EventMapNodes,
        NUMBER_OF_MODULES);
}

void Kernel_c::connect(
    KernelSwitch_c *KernelSwitch)
{
    this->KernelSwitch = KernelSwitch;
}

Error_t Kernel_c::connectModule(
    Module_c *Module)
{
    ModuleAddress_t NewModuleAddress;
    Error_t Error;

    /* Check if another module can connect */
    if(this->ModuleCount >= NUMBER_OF_MODULES)
    {
        return ERROR_NO_MODULE_SPACE_AVAILABLE;
    }    
    
    NewModuleAddress.KernelID = this->KernelID;
    NewModuleAddress.ModuleID = this->ModuleCount;    

    Module->setAddress(
        &NewModuleAddress);

    Error = Module->connect(
        this);

    if(Error != ERROR_NONE)
    {
        return Error;
    }

    this->Modules[this->ModuleCount] = Module;

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

void Kernel_c::start(void)
{
    for(ModuleID_t ModuleID = 0; ModuleID < this->ModuleCount; ModuleID++)
    {
        this->Modules[ModuleID]->start();
    }
}

Error_t Kernel_c::subscribeEvents(
    EventID_t *EventIDs, 
    size_t EventIDCount,
    ModuleID_t SubscriberID)
{
    Error_t Error;
    EventID_t NewSubscriptions[EventIDCount];
    size_t NewSubscriptionsCount = 0;

    /* Check if the subscriber ID is valid. */
    if(SubscriberID >= this->ModuleCount)
    {
        return ERROR_MODULE_ID_INVALID;
    }

    /* First, we register the module in the kernel's event map. */
    Error = this->EventMap.registerID(
        EventIDs, 
        EventIDCount, 
        SubscriberID);

    if(Error != ERROR_NONE)
    {
        return Error;
    }
    
    /* Then we check if the kernel has to subscribe to an event at the kernel switch. */
    for(size_t Count = 0; Count < EventIDCount; Count++)
    {
        /**
         * If the ID Count is exactly one, the module we just registered was the first 
         * module in the event map. In this case, the kernel needs to subscribe to the event at the kernel switch. 
         */
        if(this->EventMapNodes[EventIDs[Count]].IDCount == 1)
        {
            NewSubscriptions[NewSubscriptionsCount] = EventIDs[Count];
            NewSubscriptionsCount++;
        }
    }
    
    Error = this->KernelSwitch->subscribeEvents(
        NewSubscriptions,
        NewSubscriptionsCount,
        this->KernelID);

    if(Error != ERROR_NONE)
    {
        this->EventMap.unregisterID(
            EventIDs,
            EventIDCount,
            SubscriberID);

        return Error;
    }

    return ERROR_NONE;
}

Error_t Kernel_c::unsubscribeEvents(
    EventID_t *EventIDs, 
    size_t EventIDCount,
    ModuleID_t SubscriberID)
{
    Error_t Error;
    EventID_t NewUnsubscriptions[EventIDCount];
    size_t UnsubscriptionCount = 0;    
    
    /* Check if the subscriber ID is valid. */
    if(SubscriberID >= this->ModuleCount)
    {
        return ERROR_MODULE_ID_INVALID;
    }
    
    Error = this->EventMap.unregisterID(
        EventIDs, 
        EventIDCount, 
        SubscriberID);

    if(Error != ERROR_NONE)
    {
        return Error;
    }

    for(size_t Count = 0; Count < EventIDCount; Count++)
    {
        if(this->EventMapNodes[EventIDs[Count]].IDCount == 0)
        {
            NewUnsubscriptions[UnsubscriptionCount] = EventIDs[Count];
            UnsubscriptionCount++;
        }
    }

    Error = this->KernelSwitch->unsubscribeEvents(
        NewUnsubscriptions,
        UnsubscriptionCount,
        this->KernelID);

    if(Error != ERROR_NONE)
    {
        this->EventMap.registerID(
            EventIDs,
            EventIDCount,
            SubscriberID);

        return Error;
    }

    return ERROR_NONE;   
}

Error_t Kernel_c::offerServices(
    ServiceID_t *ServiceIDs, 
    size_t ServiceIDCount,
    ModuleAddress_t *ProviderAddress)
{    
    return this->KernelSwitch->offerService(
        ServiceIDs,
        ServiceIDCount,
        ProviderAddress);
}

Error_t Kernel_c::withdrawServices(
    ServiceID_t *ServiceIDs, 
    size_t ServiceIDCount,
    ModuleAddress_t *ProviderAddress)
{
    return this->KernelSwitch->withdrawService(
        ServiceIDs,
        ServiceIDCount,
        ProviderAddress);
}

void Kernel_c::setID(
    KernelID_t KernelID)
{
    this->KernelID = KernelID;
}

KernelID_t Kernel_c::getID(void)
{
    return this->KernelID;
}

void Kernel_c::handleMessage(
    Message_t *Message)
{
    ModuleID_t ModuleID;
    
    switch(Message->MessageHeader.MessageInformation & MESSAGE_TYPE_MASK)
    {
        case MESSAGE_TYPE_EVENT:
        {
            MapNode_t *MapNode;
            ModuleID_t ModuleID;
            
            this->EventMap.getIDs(
                (EventID_t)Message->MessageHeader.ContentID,
                &MapNode);

            for(size_t Count = 0; Count < MapNode->IDCount; Count++)
            {
                ModuleID = MapNode->IDs[Count];
                this->Modules[ModuleID]->handleEvent(Message);
            }            
        }break;

        case MESSAGE_TYPE_REQUEST:
        {
            ModuleID = Message->MessageHeader.DestinationAddress.ModuleID;
            
            this->Modules[ModuleID]->handleRequest(Message);
        }break;

        case MESSAGE_TYPE_RESPONSE:
        {
            ModuleID = Message->MessageHeader.DestinationAddress.ModuleID;
            
            this->Modules[ModuleID]->handleResponse(Message);
        }break;

        default:
        {
            NOP();
        }
    }
}

void Kernel_c::run(void)
{
    Message_t Message;

    while(true)
    {    
        if(this->KernelSwitch->getMessage(this->KernelID, &Message))
        {
            this->handleMessage(&Message);
        }
        else
        {
            IDLE(this->KernelID);
        }
    }   
}

void Kernel_c::runOnce(void)
{
    Message_t Message;

    if(this->KernelSwitch->getMessage(this->KernelID, &Message))
    {
        this->handleMessage(&Message);
    }
}
