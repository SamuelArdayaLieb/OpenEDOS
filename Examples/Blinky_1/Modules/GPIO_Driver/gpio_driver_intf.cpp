/*
 * gpio_driver_intf.cpp
 *
 *  Created on: 09.08.2022
 *      Author: samuel
 */

/* Include the interface header. */
#include "gpio_driver_intf.h"
#include <string.h>

#include <TM4C123GH6PM.h>
#include <gpio.h>
#include <sysctl.h>

/* Create a global object of the interface. */
GPIO_Driver_interface GPIO_Driver_intf;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Services ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/* SID_Toggle_GPIO_Output */

Error_t GPIO_Driver_interface::sendRequest_Toggle_GPIO_Output(
    uint32_t Port,
    uint8_t Pin)
{
    MessageHeader_t MessageHeader;

    //MessageHeader.DestinationAddress = NO_ADDRESS;
    //MessageHeader.SourceAddress = NO_ADDRESS;
    MessageHeader.MessageInformation = MESSAGE_TYPE_REQUEST;
    //MessageHeader.MessageID = 0;
    MessageHeader.ContentID = SID_Toggle_GPIO_Output;

    void *Data[] = {
        &Port,
        &Pin,
    };

    uint8_t DataSizes[] = {
        sizeof(Port),
        sizeof(Pin),
    };

    uint8_t DataCount = 2;

    return this->sendMessage(&MessageHeader, Data, DataSizes, DataCount);
}

Error_t GPIO_Driver_interface::readRequest_Toggle_GPIO_Output(
    Message_t *Message,
    uint32_t *Port,
    uint8_t *Pin)
{
    uint16_t TempSize = 0;

    memcpy(Port, &Message->Data[TempSize], sizeof(*Port));
    TempSize += sizeof(*Port);

    memcpy(Pin, &Message->Data[TempSize], sizeof(*Pin));

    return ERROR_NONE;
}


