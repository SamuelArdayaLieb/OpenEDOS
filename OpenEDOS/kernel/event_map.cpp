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

#include "include/event_map.h"
#include <string.h>

void EventMap_c::init(
    Identifier_t *MapMemory,
    MapNode_t *MapNodes, 
    size_t NumberOfColumns)
{
    this->MapMemory = MapMemory;
    this->MapNodes = MapNodes;
    this->NumberOfColumns = NumberOfColumns;

    for(size_t Count = 0; Count < NumberOfColumns*NUMBER_OF_EVENTS; Count++)
    {
        this->MapMemory[Count] = NO_ID;
    }

    /**
     * Let the map nodes point to the map memory.
     * Each map node represents one row of the map. 
     */
    for(size_t Row = 0; Row < NUMBER_OF_EVENTS; Row++)
    {
        this->MapNodes[Row].IDs = &this->MapMemory[Row*NumberOfColumns];

        this->MapNodes[Row].IDCount = 0;
    } 
}

Error_t EventMap_c::registerID(
    EventID_t *Indices, 
    size_t NumberOfIndices, 
    Identifier_t IDToBeRegistered)
{
    Error_t Error;
    size_t Count;

    /**
     * This method assumes that the passed ID to be registered is valid!
     */
    for(Count = 0; Count < NumberOfIndices; Count++)
    {
        if(Indices[Count] >= NUMBER_OF_EVENTS)
        {
            return ERROR_EVENT_ID_INVALID;
        }
    }

    for(Count = 0; Count < NumberOfIndices; Count++)
    {
        Error = this->placeID(
            Indices[Count], 
            IDToBeRegistered);

        /* Did placing the ID go wrong? */
        if(Error != ERROR_NONE)
        {
            /** 
             * Placing the ID failed. Roll back the entire registration.
             * This is done by removing the IDs that have been placed so far.
             */
            for(size_t CleanUpCount = 0; CleanUpCount < Count; CleanUpCount++)
            {
                this->removeID(
                    Indices[CleanUpCount], 
                    IDToBeRegistered);
            }
            
            /* Return the error that occured */
            return Error;
        }
    }

    return ERROR_NONE;
}

Error_t EventMap_c::unregisterID(
    EventID_t *Indices, 
    size_t NumberOfIndices, 
    Identifier_t IDToBeUnregistered)
{
    size_t Count;
    
    /** 
     * Check if the given key IDs are all valid 
     * before executing the unregistration.
     */
    for(Count = 0; Count < NumberOfIndices; Count++)
    {
        if(Indices[Count] >= NUMBER_OF_EVENTS)
        {
            return ERROR_EVENT_ID_INVALID;
        }
    }

    /* Remove the module ID from the event map */
    for(Count = 0; Count < NumberOfIndices; Count++)
    {
        this->removeID(
            Indices[Count], 
            IDToBeUnregistered);
    }

    /* Another happy return */
    return ERROR_NONE;
}

Error_t EventMap_c::getIDs(
    EventID_t Index,
    MapNode_t **MapNode)
{
    /* Check if the key ID is valid */
    if(Index >= NUMBER_OF_EVENTS)
    {
        return ERROR_EVENT_ID_INVALID;
    }
    
    /* Give the caller access to the map memory */
    *MapNode = &this->MapNodes[Index];
    
    /* That was easy */
    return ERROR_NONE;
}

Error_t EventMap_c::placeID(
    EventID_t Index,  
    Identifier_t IDToBePlaced)
{
    MapNode_t *Node = &this->MapNodes[Index];

    /* Can the map store another ID? */
    if(Node->IDCount >= this->NumberOfColumns)
    {
        /* There's no more space in the map */
        return ERROR_NO_MAP_SPACE_AVAILABLE;
    }

    /* Check if the ID is already placed */
    for(size_t Count = 0; Count < Node->IDCount; Count++)
    {
        if(Node->IDs[Count] == IDToBePlaced)
        {
            /* The ID is already registered. */
            return ERROR_NONE;
        }
    }

    /* Append the module ID to the event map node */
    Node->IDs[Node->IDCount] = IDToBePlaced;

    /* Increase the module count */
    Node->IDCount++;

    return ERROR_NONE;
}

void EventMap_c::removeID(
    EventID_t Index,  
    Identifier_t IDToBeRemoved)
{
    /**
     * This method assumes that the passed IDs are valid!
     */
        
    /* This just makes accessing the memory a little bit easier */
    MapNode_t *Node = &this->MapNodes[Index];

    /* Search for the ID to be removed in the map */
    for(size_t Count = 0; Count < Node->IDCount; Count++)
    {
        /* Did we find the ID? */
        if(Node->IDs[Count] == IDToBeRemoved)
        {
            /** 
             * To remove the ID from the map node array, we simply move down the rest of 
             * the array by one element so that the ID gets overwritten. 
             * This way the array will not get fragmented. To make this more clear it
             * is illustrated below:
             *                                                      
             *   ____0__________1__________2__________3__________4_____________
             *  |          |          |          |          |           |
             *  | some ID  | some ID  | some ID  | some ID  |   NO_ID   | ...
             *  |__________|__________|__________|__________|___________|______
             *              ´|`        ´|`     
             *         we want to       |
             *         delete this ID   |
             *                          |
             *                          |-------------------|
             *                          just move the rest 
             *              /|\         of the array          
             *               |__________|                            
             *              here, 
             *              overwriting the ID 
             *              we want to delete
             */ 
            memmove(
                &Node->IDs[Count], 
                &Node->IDs[Count+1], 
                Node->IDCount - Count - 1);

            /* Decrease the module count */
            Node->IDCount--;

            /* The last ID in the array is reset */
            Node->IDs[Node->IDCount] = NO_ID;

            break;
        }
    }
}