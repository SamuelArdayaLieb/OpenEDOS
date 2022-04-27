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

#include "include/service_map.h"
#include <string.h>

void ServiceMap_c::init(void)
{
    /* Initialize the whole service map memory */
    memset(this->ServiceMap, NO_MODULE, NUMBER_OF_SERVICES * sizeof(ModuleID_t));
}

Error_t ServiceMap_c::registerModule(ServiceID_t *ServiceIDs, size_t ServiceIDCount, 
                                     ModuleID_t ModuleID)
{
    /* Check if the module ID is valid */
    if(ModuleID >= NUMBER_OF_MODULES)
    {
        return ERROR_MODULE_ID_INVALID;
    }
    
    size_t Count;
    ServiceID_t ServiceID;

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
        if(this->ServiceMap[ServiceID] != NO_MODULE)
        {
            return ERROR_SERVICE_ALREADY_REGISTERED;
        }
    }

    /* It seems like everything is all right. Register the services. */
    for(Count = 0; Count < ServiceIDCount; Count++)
    {
        ServiceID = ServiceIDs[Count];

        this->ServiceMap[ServiceID] = ModuleID;
    }

    return ERROR_NONE;    
}

Error_t ServiceMap_c::getModuleID(ServiceID_t ServiceID, ModuleID_t *ModuleID)
{
    /* Check if the service ID is valid */
    if(ServiceID >= NUMBER_OF_SERVICES)
    {
        return ERROR_SERVICE_ID_INVALID;
    }
    
    /* Give them what they want */
    *ModuleID = this->ServiceMap[ServiceID];
    
    return ERROR_NONE;
}