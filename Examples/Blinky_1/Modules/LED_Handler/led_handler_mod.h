/*
 * led_handler_mod.h
 *
 *  Created on: 23.08.2022
 *      Author: samuel
 */

#ifndef MODULES_LED_HANDLER_LED_HANDLER_MOD_H_
#define MODULES_LED_HANDLER_LED_HANDLER_MOD_H_

/* Include the module parent class. */
#include "../../OpenEDOS/kernel/include/module.h"

class LED_Handler_module: public Module_c
{
private:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~ Inherited methods ~~~~~~~~~~~~~~~~~~~~~~~~//
    Error_t connect(Kernel_c *Kernel) override;
    Error_t init(void) override;
    void start(void) override;
    void handleEvent(Message_t *Message) override;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~ Own methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    void handleTimerTick(void);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

};



#endif /* MODULES_LED_HANDLER_LED_HANDLER_MOD_H_ */
