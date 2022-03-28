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

#ifndef EVENT_MAP_H
#define EVENT_MAP_H

/**
 * This file is the class header of the event map class. The event map is used by the
 * kernel to save which module is listening to which events. When an event occures the
 * kernel will get the list of module IDs of all the modules that have to be updated 
 * form the event map, using the event ID. 
 */

/* Needed for various defines and type definitions */
#include "defines.h"

/**
 * The memory of the event map is organized as a two dimensional array of size
 * NUMER_OF_EVENTS x NUMBER_OF_MODULES. Because the map is allocated statically, 
 * the maximum amount of memory usage is assumed. This means that the "worst" 
 * case (all modules listen to all events) is guaranteed to work. A more memory
 * efficient alternative using dynamic memory allocation might be implemented 
 * in the future.  
 * 
 * To keep track of how many module IDs are actually stored for each event ID,
 * a module count is used. This produces a little bit of memory overhead but
 * increases the time efficiency as the map doesn't have to iterate over the
 * whole array each time. To make the handling of the map easier, the module ID 
 * count is combined with a pointer to the actual module IDs in a struct.
 */
typedef struct EventMapNode_t {
    /* This points to the event map memory */
    ModuleID_t *ModuleIDs;
    /* This holds the actual amount of module IDs */
    ModuleCount_t ModuleCount;
} EventMapNode_t;

/**
 * The event map uses a two dimensional array to store the module IDs and
 * a one dimensional array of event map nodes for easier access to the memory.
 * To make this more clear it is illustrated below.
 *                                    
 *  Array index   Array of event              Two dimensional array
 *  = event ID    map nodes                   of module IDs                NUMBER_OF
 *      _         ____________                ____0__________1_____________MODULES-1_
 *     | |       |*ModuleIDs  |--points to-->| some     |          |     |           |
 *     |0|       |Count: 1    |              | ModuleID |NO_MODULE | ... | NO_MODULE |
 *     |_|       |____________|              |__________|__________|_____|___________|
 *     | |       |*ModuleIDs  |--points to-->|          |          |     |           |
 *     |1|       |Count: 0    |              |NO_MODULE |NO_MODULE | ... | NO_MODULE |
 *     |_|       |____________|              |__________|__________|_____|___________|
 *      .              .                          .          .                 .   
 *      .              .                          .          .                 .
 *      .              .                          .          .                 .   
 *      _         ____________                __________ __________ _____ ___________
 *     | |       |*ModuleIDs  |--points to-->| some     | some     |     |           |
 *     |N|       |Count: 2    |              | ModuleID | ModuleID | ... | NO_MODULE |
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
     * This function initializes the event map. This means that the memory used
     * by the event map is set to initial values. The event map nodes are then
     * set to be pointing to the event map memory.
     */
    void init(void);

    /**
     * @brief Register a module for a given list of event IDs.
     * 
     * This function is part of the module registration at the kernel. The module
     * tells the kernel which events it wants to be informed about by passing a list
     * of event IDs. The ID of the module is then placed in the event map using each
     * event ID as a map index. If the module is already registered for a specific
     * event, the event map will return an error. The event map memory is big enough 
     * to store all module IDs for all event IDs. Nevertheless the event map checks 
     * and returns an error if there is no space left (this should never happen as 
     * multiple registrations by the same module for the same event are not possible).   
     * 
     * @param EventIDs This is the pointer to an array of event IDs, provided by the
     * module that wants to be informed about those events.
     * 
     * @param EventIDCount This holds the number of event IDs in the array (i. e. the
     * size of the array).
     *  
     * @param ModuleID This is the ID of the module that wants to be registered for 
     * the given event IDs.
     * 
     * @return Error_t An error is returned if
     * - the module ID is invalid.
     * - one of the event IDs is invalid.
     * - the module ID is already registered for one of the event IDs.
     * - there is no space left in the event map for one of the event IDs.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t registerModule(EventID_t *EventIDs, size_t EventIDCount, ModuleID_t ModuleID);

    /**
     * @brief Unregister a module for a given list of event IDs.
     * 
     * This is basically the opposite of registerModule(). It is called if the kernel
     * encounters an error during the module registration in order to roll back the
     * registration. The passed module ID will be removed from every line of the 
     * event map, using the passed event IDs as map indeces. This means that the 
     * module will no longer be informed about those events. Trying to unregister
     * a module that has not been registered before will NOT result in an error.
     * 
     * @param EventIDs This is the pointer to an array of events, provided by the 
     * module that will no longer be informed about those events.
     * 
     * @param EventIDCount This holds the number of event IDs in the array (i. e. the
     * size of the array).
     * 
     * @param ModuleID This is the ID of the module that wants/has to be unregistered
     * for the given event IDs.
     *  
     * @return Error_t An error is returned if
     * - one of the event IDs is invalid.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t unregisterModule(EventID_t *EventIDs, size_t EventIDCount, ModuleID_t ModuleID);

    /**
     * @brief Get the list of module IDs which are registered for a given event ID.
     * 
     * This function is used by the kernel to get the list of IDs of the modules 
     * that have to be informed about an occured event. The ID of the occured event
     * is passed alongside a pointer to a pointer to an event map node. The event map
     * can simply assign the address of the actual event map node associated with the 
     * given event ID to the pointer pointed to by the given pointer. This way the 
     * kernel can access the memory holding the module IDs.
     * 
     * @param EventID This is the ID of the event that the kernel wants to inform
     * modules about. The event map will provide the list of all module IDs that 
     * are registered for this event ID by letting the kernel access the memory.
     * 
     * @param EventMapNode This points to the pointer to an event map node.
     * The event map will assign the address of the actual event map node of the 
     * given event ID to the pointer this pointer points to. 
     * 
     * @return Error_t An error is returned if
     * - the event ID is invalid.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t getModuleList(EventID_t EventID, EventMapNode_t **EventMapNode);

private:
    /**
     * @brief Place a module ID in the event map.
     * 
     * This function is used internally during registerModule(). For each of the 
     * given event IDs in registerModule() this function will be called. It places
     * a given module ID in the event map using the given event ID as the map index.
     * The function first checks if the IDs are invalid, if there is no space in 
     * the event map and if the module ID has already been placed in the map. If
     * none of this applies the module ID is stored in memory and the module count 
     * for the given event ID is increased. 
     * 
     * @param EventID The event ID that the module will be registered for.
     * 
     * @param ModuleID The module ID that will be registered.
     * 
     * @return Error_t An error is returned if
     * - the module ID is invalid.
     * - the event ID is invalid.
     * - there is no more space in the event map.
     * - the module ID is already placed in the event map.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t placeModuleID(EventID_t EventID, ModuleID_t ModuleID);

    /**
     * @brief Remove a module ID from the event map.
     * 
     * This function is basically the opposite of placeModuleID(). It is used 
     * internally during unregisterModule(). For each of the given event IDs in
     * unregisterModule() this function will be called. It removes a given module ID
     * from the event map using the given event ID as the map index. This function
     * first checks if the IDs are invalid. If this doesn't apply the module ID
     * is removed from the list and the module count for the given event ID is
     * decreased. This is done in a way so that the list doesn't get fragmented.
     * This means that if there are N module IDs stored in the event map, they will
     * always be stored in the first N elements of the array. 
     * 
     * @param EventID The event ID that the module will no longer be registered for.
     * 
     * @param ModuleID The module ID that will be removed.
     * 
     * @return Error_t An error is returned if
     * - the event ID is invalid.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t removeModuleID(EventID_t EventID, ModuleID_t ModuleID);

    /**
     * The event map memory is a two dimensional array of type ModuleID_t in which
     * all the registered module IDs are stored. Each line of this matrix represents
     * one specific event. This way an event ID can be used as the index to directly
     * access the memory.
     */
    ModuleID_t EventMapMemory[NUMBER_OF_EVENTS][NUMBER_OF_MODULES];

    /**
     * The event map is an array of type EventMapNode_t where each element of the
     * array represents one line of the event map and thus one specific event. 
     * This way an event ID can be used as the index to directly access the needed
     * event map node.
     */
    EventMapNode_t EventMap[NUMBER_OF_EVENTS];
};

#endif