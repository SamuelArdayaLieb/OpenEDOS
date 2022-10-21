/*
 * led_handler_mod.cpp
 *
 *  Created on: 23.08.2022
 *      Author: samuel
 */

/* Include the module header. */
#include "led_handler_mod.h"
#include "../../OpenEDOS/kernel/include/kernel.h"

#include "../Timer_Driver/timer_driver_intf.h"
#include "../GPIO_Driver/gpio_driver_intf.h"

#include "../../drivers/rgb.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Inherited methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Error_t LED_Handler_module::connect(Kernel_c *Kernel)
{
    Error_t Error;

    // List the events to which this module will be listening
    EventID_t EventIDs[] = {
        EID_Timer_Tick,
    };

    // List the services this module is offering
    ServiceID_t ServiceIDs[] = {
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

Error_t LED_Handler_module::init(void)
{
    return ERROR_NONE;
}

void LED_Handler_module::start(void)
{
    Timer_Driver_intf.sendRequest_Start_Timer();
}

void LED_Handler_module::handleEvent(Message_t *Message)
{
    switch(Message->MessageHeader.ContentID)
    {
        case EID_Timer_Tick:
        {
            this->handleTimerTick();
        }break;
    }
}

void LED_Handler_module::handleTimerTick(void)
{
    GPIO_Driver_intf.sendRequest_Toggle_GPIO_Output(RED_GPIO_BASE, RED_GPIO_PIN);
}
