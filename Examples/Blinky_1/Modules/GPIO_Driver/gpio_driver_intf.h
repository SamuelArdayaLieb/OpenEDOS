#ifndef GPIO_DRIVER_INTF_H
#define GPIO_DRIVER_INTF_H

#include "../../OpenEDOS/kernel/include/interface.h"

class GPIO_Driver_interface: public Interface_c
{
public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Services ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

    /* SID_Toggle_GPIO_Output */

    /**
     * @brief Send request for service: Toggle_GPIO_Output
     *
     * Toggle the output of a GPIO.
     * 
     * Response expected: False
     *
     * @param Port The port of the GPIO to be toggled.
     * Use the macros defined in gpio.h.
     * 
     * @param Pin The pin of the GPIO to be toggled.
     * Use the macros defined in gpio.h.
     * 
     * @return Error_t An error is returned if
     * - sending the message results in an error.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t sendRequest_Toggle_GPIO_Output(
        uint32_t Port,
        uint8_t Pin);

    /**
     * @brief Read request for service: Toggle_GPIO_Output
     *
     * Use this function to retrieve information from a received request message.
     *
     * @param Message Pointer to the received message object.
     *
     * @param Port The port of the GPIO to be toggled.
     * Use the macros defined in gpio.h.
     * 
     * @param Pin The pin of the GPIO to be toggled.
     * Use the macros defined in gpio.h.
     * 
     * @return Error_t An error is returned if
     * - retrieving data from the message results in an error.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t readRequest_Toggle_GPIO_Output(
        Message_t *Message,
        uint32_t *Port,
        uint8_t *Pin);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Events ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

    /* No events are associated with this module. */

};

extern GPIO_Driver_interface GPIO_Driver_intf;

#endif//GPIO_DRIVER_INTF_H

