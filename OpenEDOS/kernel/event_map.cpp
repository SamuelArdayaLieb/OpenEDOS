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

#include "include/event_map.h"
#include <string.h>

void EventMap_c::init(void)
{
     /**
     * Set the event map memory to initial values.
     * Let the event map nodes point to the acutal event map memory.
     * Each map node represents one line of the map. 
     */
    for(size_t Line = 0; Line < NUMBER_OF_EVENTS; Line++)
    {
        memset(this->EventMapMemory[Line], NO_MODULE, NUMBER_OF_MODULES * sizeof(ModuleID_t));

        this->EventMap[Line].ModuleIDs = this->EventMapMemory[Line];
        
        this->EventMap[Line].ModuleCount = 0;
    }    
}

Error_t EventMap_c::registerModule(EventID_t *EventIDs, size_t EventIDCount, 
                                   ModuleID_t ModuleID)
{
    /* Check if the module ID is valid */
    if(ModuleID >= NUMBER_OF_MODULES)
    {
        return ERROR_MODULE_ID_INVALID;
    }

    size_t Count;

    /** 
     * Check if the given event IDs are all valid 
     * before executing the registration.
     */
    for(Count = 0; Count < EventIDCount; Count++)
    {
        if(EventIDs[Count] >= NUMBER_OF_EVENTS)
        {
            return ERROR_EVENT_ID_INVALID;
        }
    }

    /* So far so good... Let's see if we need this */
    Error_t Error;

    /**
     * Now place every module ID in the map lines associated
     * with the given event IDs.
     */
    for(Count = 0; Count < EventIDCount; Count++)
    {
        Error = this->placeModuleID(EventIDs[Count], ModuleID);

        /* Did placing the module ID go wrong? */
        if(Error != ERROR_NONE)
        {
            /** 
             * Placing the module ID failed. Roll back the entire registration.
             * This is done by removing the IDs that have been placed so far.
             */
            for(size_t CleanUpCount = 0; CleanUpCount < Count; CleanUpCount++)
            {
                this->removeModuleID(EventIDs[CleanUpCount], ModuleID);
            }
            
            /* Return the error that occured */
            return Error;
        }
    }    

    /* Phew... All went good */
    return ERROR_NONE;
}

Error_t EventMap_c::unregisterModule(EventID_t *EventIDs, size_t EventIDCount, 
                                     ModuleID_t ModuleID)
{
    /** 
     * Checking if the module ID is invalid is not necessary here
     * as it wouldn't have any consequences. 
     */

    size_t Count;
    
    /** 
     * Check if the given event IDs are all valid 
     * before executing the unregistration.
     */
    for(Count = 0; Count < EventIDCount; Count++)
    {
        if(EventIDs[Count] >= NUMBER_OF_EVENTS)
        {
            return ERROR_EVENT_ID_INVALID;
        }
    }

    /* Remove the module ID from the event map */
    for(Count = 0; Count < EventIDCount; Count++)
    {
        this->removeModuleID(EventIDs[Count], ModuleID);
    }

    /* Another happy return */
    return ERROR_NONE;
}

Error_t EventMap_c::getModuleList(EventID_t EventID, EventMapNode_t **EventMapNode)
{
    /* Check if the event ID is valid */
    if(EventID >= NUMBER_OF_EVENTS)
    {
        return ERROR_EVENT_ID_INVALID;
    }
    
    /* Give the caller access to the event map memory */
    *EventMapNode = &this->EventMap[EventID];
    
    /* That was easy */
    return ERROR_NONE;   
}

Error_t EventMap_c::placeModuleID(EventID_t EventID, ModuleID_t ModuleID)
{
    /* Check if the event ID is valid */
    if(EventID >= NUMBER_OF_EVENTS)
    {
        return ERROR_EVENT_ID_INVALID;
    }

    /* Check if the module ID is valid */
    if(ModuleID >= NUMBER_OF_MODULES)
    {
        return ERROR_MODULE_ID_INVALID;
    }
    
    /* This just makes accessing the memory a little bit easier */
    EventMapNode_t *Node = &this->EventMap[EventID];

    /* Can the event map store another ID? */
    if(Node->ModuleCount >= NUMBER_OF_MODULES)
    {
        /* There's no more space in the map */
        return ERROR_NO_MAP_SPACE_AVAILABLE;
    }

    /* Check if the module ID is already placed */
    for(ModuleCount_t Count = 0; Count < Node->ModuleCount; Count++)
    {
        if(Node->ModuleIDs[Count] == ModuleID)
        {
            /* The module is already registered for the event */
            return ERROR_MODULE_ALREADY_REGISTERED;
        }
    }

    /* Append the module ID to the event map node */
    Node->ModuleIDs[Node->ModuleCount] = ModuleID;

    /* Increase the module count */
    Node->ModuleCount++;

    return ERROR_NONE;    
}

Error_t EventMap_c::removeModuleID(EventID_t EventID, ModuleID_t ModuleID)
{
    /** 
     * Checking if the module ID is invalid is not necessary here
     * as it wouldn't have any consequences. 
     */
    
    /* Check if the event ID is valid */
    if(EventID >= NUMBER_OF_EVENTS)
    {
        return ERROR_EVENT_ID_INVALID;
    }
    
    /* This just makes accessing the memory a little bit easier */
    EventMapNode_t *Node = &this->EventMap[EventID];

    /* Search for the module ID in the event map */
    for(ModuleCount_t Count = 0; Count < Node->ModuleCount; Count++)
    {
        /* Did we find the module ID? */
        if(Node->ModuleIDs[Count] == ModuleID)
        {
            /** 
             * To remove the module ID from the array, we simply move down the rest of 
             * the array by one element so that the module ID gets overwritten. 
             * This way the array will not get fragmented. To make this more clear it
             * is illustrated below:
             *                                                      
             *   ____0__________1__________2__________3__________4_____________
             *  | some     | some     | some     | some     |           |
             *  | ModuleID | ModuleID | ModuleID | ModuleID | NO_MODULE | ...
             *  |__________|__________|__________|__________|___________|______
             *              ´|`        ´|`     
             *           we want to     |
             *           delete this    |
             *           module ID      |
             *                          |-------------------|
             *                          just move the rest 
             *              /|\         of the array          
             *               |__________|                            
             *              here, 
             *              overwriting the ID 
             *              we want to delete
             */ 
            memmove(&Node->ModuleIDs[Count], &Node->ModuleIDs[Count+1], 
                    Node->ModuleCount - Count - 1);

            /* The last ID in the list is reset */
            Node->ModuleIDs[Node->ModuleCount] = NO_MODULE;

            /* Decrease the module count */
            Node->ModuleCount--;

            break;
        }
    }

    /* That's it */
    return ERROR_NONE;   
}