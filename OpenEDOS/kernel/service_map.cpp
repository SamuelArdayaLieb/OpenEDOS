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

#include "include/service_map.h"
#include <string.h>

void ServiceMap_c::init(void)
{
    ModuleAddress_t NoProvider = {
        NO_KERNEL,
        NO_MODULE
    };
    
    for(ServiceID_t ServiceID = 0; ServiceID < NUMBER_OF_SERVICES; ServiceID++)
    {
        this->ServiceMap[ServiceID] = NoProvider;
    }
}

Error_t ServiceMap_c::registerServiceProvider(
    ServiceID_t *ServiceIDs, 
    size_t ServiceIDCount,
    ModuleAddress_t *ProviderAddress)
{
    size_t Count;
    ServiceID_t ServiceID;
    
    /* Check if the provider address is valid */
    if(ProviderAddress->KernelID >= NUMBER_OF_KERNELS)
    {
        return ERROR_KERNEL_ID_INVALID;
    }

    if(ProviderAddress->ModuleID >= NUMBER_OF_MODULES)
    {
        return ERROR_MODULE_ID_INVALID;
    }
    
    /** 
     * Check if the given service IDs are all valid before executing the registration.
     * Also check if any of the given service IDs is already registered by another 
     * module.
     */
    for(Count = 0; Count < ServiceIDCount; Count++)
    {
        ServiceID = ServiceIDs[Count];
        
        /* Is the service ID valid? */
        if(ServiceID >= NUMBER_OF_SERVICES)
        {
            return ERROR_SERVICE_ID_INVALID;
        }

        /* Is the service already registered? */
        if(this->ServiceMap[ServiceID].ModuleID != NO_MODULE)
        {
            return ERROR_SERVICE_ALREADY_REGISTERED;
        }
    }

    /* It seems like everything is all right. Let's register the services. */
    for(Count = 0; Count < ServiceIDCount; Count++)
    {
        ServiceID = ServiceIDs[Count];

        this->ServiceMap[ServiceID] = *ProviderAddress;
    }

    return ERROR_NONE;    
}

Error_t ServiceMap_c::unregisterServiceProvider(
    ServiceID_t *ServiceIDs, 
    size_t ServiceIDCount,
    ModuleAddress_t *ProviderAddress)
{
    size_t Count;
    ServiceID_t ServiceID;
    ModuleAddress_t NoProvider = {
        NO_KERNEL,
        NO_MODULE
    };
    
    /* Check if the provider address is valid */
    if(ProviderAddress->KernelID >= NUMBER_OF_KERNELS)
    {
        return ERROR_KERNEL_ID_INVALID;
    }

    if(ProviderAddress->ModuleID >= NUMBER_OF_MODULES)
    {
        return ERROR_MODULE_ID_INVALID;
    }

    /** 
     * Check if the given service IDs are all valid before executing the unregistration.
     * Also check if any of the given service IDs is registered by another provider.
     */
    for(Count = 0; Count < ServiceIDCount; Count++)
    {
        ServiceID = ServiceIDs[Count];
        
        /* Is the service ID valid? */
        if(ServiceID >= NUMBER_OF_SERVICES)
        {
            return ERROR_SERVICE_ID_INVALID;
        }

        /* Is the service registered by another module? */
        if((this->ServiceMap[ServiceID].KernelID != ProviderAddress->KernelID)
         ||(this->ServiceMap[ServiceID].ModuleID != ProviderAddress->ModuleID))
        {
            return ERROR_ILLEGAL_ACTION;
        }
    }

    /* It seems like everything is all right. Unregister the services. */
    for(Count = 0; Count < ServiceIDCount; Count++)
    {
        ServiceID = ServiceIDs[Count];

        this->ServiceMap[ServiceID] = NoProvider;
    }

    return ERROR_NONE;    
}


Error_t ServiceMap_c::getServiceProvider(
    ServiceID_t ServiceID, 
    ModuleAddress_t *ServiceProvider)
{
    /* Check if the service ID is valid */
    if(ServiceID >= NUMBER_OF_SERVICES)
    {
        return ERROR_SERVICE_ID_INVALID;
    }
    
    /* Give them what they want */
    *ServiceProvider = this->ServiceMap[ServiceID];
    
    return ERROR_NONE;
}