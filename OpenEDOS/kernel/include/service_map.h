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

#ifndef SERVICE_MAP_H
#define SERVICE_MAP_H

/**
 * This file is the class header of the service map class. The service map is used by 
 * the kernel to save which module is responsible for which service. When a message
 * needs to be forwarded the kernel gets the ID of the one module that the message
 * has to be directed to according to the service ID which the message header 
 * contains. Following the single responsibility principle, there has to be exactly
 * one specific module registered for one specific service.   
 */

/* Needed for various defines and type definitions */
#include "defines.h"

/**
 * The memory of the service map is a statically allocated one dimensional array
 * of size NUMBER_OF_SERVICES. It maps each service ID to one module ID, using the
 * service ID as the map index.
 */

class ServiceMap_c
{
public:
    /**
     * @brief Initialize the service map.
     * 
     * This function initializes the service map. This means that the memory used
     * by the service map is set to initial values.
     */
    void init(void);

    /**
     * @brief Register a module for a given list of service IDs.
     * 
     * This function is part of the module registration at the kernel. The module
     * tells the kernel which services it wants to offer by passing a list of 
     * service IDs. The ID of the module is then placed in the service map using the
     * service ID as a map index. The service map memory can store exactly one 
     * module ID for each service ID. If another module is already registered for 
     * the given service, the service map will return an error. If this happens,
     * the whole registration of the module will be rolled back. 
     * 
     * @param ServiceIDs This is the pointer to an array of service IDs, provided by
     * the module that wants to offer those services.
     * 
     * @param ServiceIDCount This holds the number of service IDs in the array 
     * (i. e. the size of the array).
     * 
     * @param ModuleID 
     * @return Error_t An error is returned if
     * - the module ID is invalid.
     * - one of the service IDs is invalid.
     * - another module is already registered for one of the service IDs.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t registerModule(ServiceID_t *ServiceIDs, size_t ServiceIDCount, 
                           ModuleID_t ModuleID);

    /**
     * @brief Get the module ID which is registered for a given service ID.
     * 
     * This function is used by the kernel to get the ID of the module that a
     * message has to be forwarded to. The ID of the service is passed alongside a 
     * pointer to a module ID. The event map can simply assign the needed module ID 
     * to the dereferenced pointer. This way the kernel gets the ID.
     * 
     * @param ServiceID The service ID for which the responsible module is seeked.
     * 
     * @param ModuleID This pointer to a module ID is used to pass the seeked ID
     * to the caller of the function.
     *  
     * @return Error_t An error is returned if
     * - the service ID is invalid.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t getModuleID(ServiceID_t ServiceID, ModuleID_t *ModuleID);

private:
    /**
     * The service map memory is a one dimensional array of type ModuleID_t.
     * Each element of the array represents one specifig service. This way
     * a service ID can be used as the index to directly access the needed
     * module ID.
     */
    ModuleID_t ServiceMap[NUMBER_OF_SERVICES];
};

#endif