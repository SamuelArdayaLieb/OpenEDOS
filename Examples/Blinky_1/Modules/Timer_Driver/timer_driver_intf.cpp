/*
 * timer_driver_intf.cpp
 *
 *  Created on: 09.08.2022
 *      Author: samuel
 */

/* Include the interface header. */
#include "timer_driver_intf.h"
#include <string.h>

/* Create a global object of the interface. */
Timer_Driver_interface Timer_Driver_intf;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Services ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

Error_t Timer_Driver_interface::sendEvent_Timer_Tick(void)
{
    MessageHeader_t MessageHeader;

    MessageHeader.MessageInformation = MESSAGE_TYPE_EVENT;
    MessageHeader.ContentID = EID_Timer_Tick;

    return this->sendMessage(&MessageHeader, NULL, NULL, 0);
}

Error_t Timer_Driver_interface::sendRequest_Start_Timer(void)
{
    MessageHeader_t MessageHeader;

    MessageHeader.MessageInformation = MESSAGE_TYPE_REQUEST;
    MessageHeader.ContentID = SID_Start_Timer;

    return this->sendMessage(&MessageHeader, NULL, NULL, 0);
}
