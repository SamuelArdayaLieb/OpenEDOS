/*
 * gpio_driver_mod.cpp
 *
 *  Created on: 09.08.2022
 *      Author: samuel
 */

/* Include the module header. */
#include "gpio_driver_mod.h"

/* Include hardware specific headers. */
#include <TM4C123GH6PM.h>
#include <gpio.h>
#include <sysctl.h>
#include "inc/hw_memmap.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Inherited methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Error_t GPIO_Driver_module::connect(Kernel_c *Kernel)
{
    Error_t Error;

    // List the events to which this module will be listening
    EventID_t EventIDs[] = {
    };

    // List the services this module is offering
    ServiceID_t ServiceIDs[] = {
        SID_Toggle_GPIO_Output,
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

Error_t GPIO_Driver_module::init(void)
{
    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    //
    // Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    return ERROR_NONE;
}

void GPIO_Driver_module::handleRequest(Message_t *Message)
{
    switch(Message->MessageHeader.ContentID)
    {
        case SID_Toggle_GPIO_Output:
        {
            uint32_t Port;
            uint8_t Pin;

            GPIO_Driver_intf.readRequest_Toggle_GPIO_Output(Message, &Port, &Pin);

            GPIOPinToggle(Port, Pin);
        }break;

        default:
        {
            NOP();
        }
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Own methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/* This module has no own methods. */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ISRs and callback functions ~~~~~~~~~~~~~~~~~~~//
/* There are no ISRs or callback functions associated with this module. */


