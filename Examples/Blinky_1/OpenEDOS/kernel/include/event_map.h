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

#ifndef MAP_H
#define MAP_H

/**
 * This file is the class header of the event map class. The event map is used by a
 * kernel to save which module is subscribed to which events.  When an event occures, 
 * the kernel gets the list of module IDs of all the subscribed modules using the 
 * event ID. The kernel switch also has an event map in order to forward event messages 
 * to kernels.
 */

#include "defines.h"

/**
 * The memory of the event map is organized as a two dimensional array of size
 * NUMER_OF_EVENTS x NumberOfColumns where the number of columns is variable. 
 * To keep track of how many IDs are currently stored for each event ID,
 * an ID count is used. To make the handling of the map easier, the ID count 
 * is combined with a pointer to the IDs in a struct.
 */
typedef struct MapNode_t {
    /* This points to the map memory */
    Identifier_t *IDs;
    /* This holds the actual amount of IDs */
    size_t IDCount;
} MapNode_t;

/**
 * The event map uses a two dimensional array to store the identifiers and
 * a one dimensional array of map nodes for easier access to the memory.
 * To make this more clear it is illustrated below.
 *                                    
 *  Array index   Array of                    Two dimensional array
 *  = event ID    map nodes                   of identifiers               Number of
 *      _         ____________                ____0__________1_____________Columns-1_
 *     | |       |*IDs        |--points to-->|          |          |     |           |
 *     |0|       |Count: 1    |              | some ID  |  NO_ID   | ... |   NO_ID   |
 *     |_|       |____________|              |__________|__________|_____|___________|
 *     | |       |*IDs        |--points to-->|          |          |     |           |
 *     |1|       |Count: 0    |              |  NO_ID   |  NO_ID   | ... |   NO_ID   |
 *     |_|       |____________|              |__________|__________|_____|___________|
 *      .              .                          .          .                 .   
 *      .              .                          .          .                 .
 *      .              .                          .          .                 .   
 *      _         ____________                __________ __________ _____ ___________
 *     | |       |*IDs        |--points to-->|          |          |     |           |
 *     |N|       |Count: 2    |              | some ID  | some ID  | ... |   NO_ID   |
 *     |_|       |____________|              |__________|__________|_____|___________|         
 *        
 *      N =       
 *  NUMBER_OF_EVENTS - 1               
 */

class EventMap_c
{
public:
    /**
     * @brief Initialize the event map.
     * 
     * The memory used by the event map is set to initial values. 
     * The event map nodes are then set to be pointing to the event map memory.
     * 
     * @param MapMemory The memory used by the map.
     * 
     * @param MapNodes The array of map nodes used by the map.
     * 
     * @param NumberOfColumns The number of colums in the map.
     */
    void init(
        Identifier_t *MapMemory,
        MapNode_t *MapNodes, 
        size_t NumberOfColumns);

    /**
     * @brief Register an identifier for a given list of event IDs.
     * 
     * The ID to be registered is placed in the event map using each event ID as a 
     * map index. The event map checks and returns an error if there is no space left. 
     * Trying to register a module that is already registered does not have any effect.
     * 
     * @param Indices Pointer to an array of event IDs used as map indices.
     * 
     * @param NumberOfIndices The number of indices in the array.
     * 
     * @param IDToBeRegistered The ID that is registered for 
     * the given event IDs.
     * 
     * @return Error_t An error is returned if
     * - one of the event IDs is invalid.
     * - there is no space left in the event map for one of the event IDs.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t registerID(
        EventID_t *Indices, 
        size_t NumberOfIndices, 
        Identifier_t IDToBeRegistered);

    /**
     * @brief Unregister an identifier for a given list of event IDs.
     * 
     * The ID to be unregistered will be removed from every line of the 
     * event map, using the passed event IDs as map indeces.
     * 
     * @param Indices Pointer to an array of event IDs used as map indices.
     * 
     * @param NumberOfIndices The number of indices in the array.
     * 
     * @param IDToBeRegistered The ID that is unregistered for 
     * the given event IDs. 
     * 
     * @return Error_t An error is returned if
     * - one of the event IDs is invalid.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t unregisterID(
        EventID_t *Indices, 
        size_t NumberOfIndices, 
        Identifier_t IDToBeUnregistered);

    /**
     * @brief Get the map node associated to a given event ID.
     * 
     * The event map assigns the address of the map node associated with the 
     * given event ID to the given pointer. This way the memory holding the module IDs
     * can be accessed.
     * 
     * @param Index The event ID for the requested map node.
     * 
     * @param MapNode The map node associated to the given event ID.
     * 
     * @return Error_t An error is returned if
     * - the event ID is invalid.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t getIDs(
        EventID_t Index,
        MapNode_t **MapNode);

private:
    /**
     * @brief Place an identifier in the event map.
     * 
     * This function places a given module ID in the event map using the given
     * event ID as the map index.
     * 
     * @param Index The event ID the ID to be placed is registered for.
     * 
     * @param IDToBePlaced The ID to be placed.
     * 
     * @return Error_t An error is returned if
     * - there is no more space in the event map.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t placeID(
        EventID_t Index,  
        Identifier_t IDToBePlaced);

    /**
     * @brief Remove an identifier from the event map.
     * 
     * This function removes a given module ID from the event map using the given 
     * event ID as the map index.
     * 
     * @param Index The event ID that the ID to be removed is no longer registered for.
     * 
     * @param IDToBeRemoved The ID to be removed. 
     */
    void removeID(
        EventID_t Index,  
        Identifier_t IDToBeRemoved);

    /* The memory used by the event map has to be provided. */
    Identifier_t *MapMemory;
    MapNode_t *MapNodes;
    size_t NumberOfColumns;
};

#endif//MAP_H