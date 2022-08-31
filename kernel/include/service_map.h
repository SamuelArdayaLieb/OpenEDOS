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

#ifndef SERVICE_MAP_H
#define SERVICE_MAP_H

/**
 * This file is the class header of the service map class. The service map is used by 
 * the kernel switch to save which module is responsible for which service. When a message
 * needs to be forwarded the kernel switch gets the address of the one module that the message
 * has to be directed to according to the service ID. Following the single responsibility 
 * principle, there has to be exactly one specific module registered for one specific 
 * service.   
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
     * A module tells the kernel which services it wants to offer by passing a list of 
     * service IDs. The kernel forwards this offer to the kernel switch. The address of 
     * the module is then placed in the service map using the service ID as a map index. 
     * The service map memory can store exactly one module address for each service ID. 
     * If another module is already registered for the given service, the service map will 
     * return an error. If this happens, the whole registration of the module is rolled back. 
     * 
     * @param ServiceIDs The pointer to an array of service IDs, provided by
     * the module that wants to offer those services.
     * 
     * @param ServiceIDCount The number of service IDs in the array.
     * 
     * @param ProviderAddress A pointer to the address of the module that offers services.
     * 
     * @return Error_t An error is returned if
     * - the module address is invalid.
     * - one of the service IDs is invalid.
     * - another module is already registered for one of the service IDs.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t registerServiceProvider(
        ServiceID_t *ServiceIDs, 
        size_t ServiceIDCount,
        ModuleAddress_t *ProviderAddress);

    /**
     * @brief Unregister a module for a given list of service IDs.
     * 
     * This function is used to delete the link between a module and a service ID.
     * The module address will be removed from the service map for every service ID passed
     * in the list. Once this is done, the kernel switch will no longer forward associated
     * service requests to the module. If an error occures during the unregistration,
     * the whole process is canceled and the service map is left unchanged. 
     * 
     * @param ServiceIDs This is the pointer to an array of service IDs, provided by
     * the module that wants to unregister those services.
     *  
     * @param ServiceIDCount This holds the number of service IDs in the array.
     * 
     * @param ProviderAddress The address of the module that unregisters services.
     * 
     * @return Error_t An error is returned if
     * - the module address is invalid.
     * - one of the service IDs is invalid.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t unregisterServiceProvider(
        ServiceID_t *ServiceIDs, 
        size_t ServiceIDCount,
        ModuleAddress_t *ProviderAddress);

    /**
     * @brief Get the module adress which is registered for a given service ID.
     * 
     * This function is used by the kernel switch to get the address of the module 
     * that a message has to be forwarded to. 
     * 
     * @param ServiceID The service ID for which the responsible module is seeked.
     * 
     * @param ProviderAddress The pointer to a module address used to pass the seeked
     * address to the caller of the function.
     *  
     * @return Error_t An error is returned if
     * - the service ID is invalid.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t getServiceProvider(
        ServiceID_t ServiceID, 
        ModuleAddress_t *ProviderAddress);

private:
    /**
     * The service map memory is a one dimensional array of type ModuleAddress_t.
     * There is one module address for each service.
     */
    ModuleAddress_t ServiceMap[NUMBER_OF_SERVICES];
};

#endif//SERVICE_MAP_H