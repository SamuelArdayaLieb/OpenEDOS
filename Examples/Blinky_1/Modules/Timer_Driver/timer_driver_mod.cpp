/*
 * timer_driver_mod.cpp
 *
 *  Created on: 09.08.2022
 *      Author: samuel
 */

/* Include the module header. */
#include "timer_driver_mod.h"

/* Include hardware specific headers. */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Inherited methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Error_t Timer_Driver_module::connect(
    Kernel_c *Kernel)
{
    Error_t Error;

    // List the events to which this module will be listening
    EventID_t EventIDs[] = {
    };

    // List the services this module is offering
    ServiceID_t ServiceIDs[] = {
        SID_Start_Timer,
    };

    Error = Kernel->subscribeEvents(
        EventIDs, 
        sizeof(EventIDs), 
        this->ModuleAddress.ModuleID);

    // Check for errors
    if(Error != ERROR_NONE)
    {
        return Error;
    }

    Error = Kernel->offerServices(
        ServiceIDs, 
        sizeof(ServiceIDs), 
        &this->ModuleAddress);

    // Check for errors
    if(Error != ERROR_NONE)
    {
        return Error;
    }

    // Set the connection to the kernel
    this->Kernel = Kernel;

    return ERROR_NONE;
}

Error_t Timer_Driver_module::init(void)
{
    return ERROR_NONE;
}

void Timer_Driver_module::handleRequest(
    Message_t *Message)
{
    switch(Message->MessageHeader.ContentID)
    {
        case SID_Start_Timer:
        {
            this->startTimer();
        } break;

        default:
        {
            NOP();
        }
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Own methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void Timer_Driver_module::startTimer(void)
{
    volatile uint32_t i;

    while(true)
    {
        i = 2000000;

        while(i)
        {
            i--;
        }

        Timer_Driver_intf.sendEvent_Timer_Tick();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ISRs and callback functions ~~~~~~~~~~~~~~~~~~~//
/* There are no ISRs or callback functions associated with this module. */


