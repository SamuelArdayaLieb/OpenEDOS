/*
 * gpio_driver_mod.h
 *
 *  Created on: 09.08.2022
 *      Author: samuel
 */

#ifndef MODULES_GPIO_DRIVER_GPIO_DRIVER_MOD_H_
#define MODULES_GPIO_DRIVER_GPIO_DRIVER_MOD_H_

/* Include the module parent class. */
#include "../../OpenEDOS/kernel/include/module.h"

/* Include the corresponding interface. */
#include "gpio_driver_intf.h"

class GPIO_Driver_module: public Module_c
{
private:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~ Inherited methods ~~~~~~~~~~~~~~~~~~~~~~~~//
    Error_t connect(Kernel_c *Kernel) override;
    Error_t init(void) override;
    void handleRequest(Message_t *Message) override;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~ Own methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    /* This module has no own methods. */

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

};



#endif /* MODULES_GPIO_DRIVER_GPIO_DRIVER_MOD_H_ */
