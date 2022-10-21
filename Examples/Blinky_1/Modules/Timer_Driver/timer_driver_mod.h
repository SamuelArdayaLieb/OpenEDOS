/*
 * timer_driver_mod.h
 *
 *  Created on: 09.08.2022
 *      Author: samuel
 */

#ifndef MODULES_TIMER_DRIVER_TIMRE_DRIVER_MOD_H_
#define MODULES_TIMER_DRIVER_TIMRE_DRIVER_MOD_H_

/* Include the module parent class. */
#include "../../OpenEDOS/kernel/include/module.h"

/* Include the corresponding interface. */
#include "timer_driver_intf.h"

class Timer_Driver_module: public Module_c
{
private:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~ Inherited methods ~~~~~~~~~~~~~~~~~~~~~~~~//
    Error_t connect(Kernel_c *Kernel) override;
    Error_t init(void) override;
    void handleRequest(Message_t *Message) override;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~ Own methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
    void startTimer(void);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

};



#endif /* MODULES_TIMER_DRIVER_TIMRE_DRIVER_MOD_H_ */
