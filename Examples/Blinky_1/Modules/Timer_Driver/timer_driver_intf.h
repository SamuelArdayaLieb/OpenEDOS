#ifndef TIMER_DRIVER_INTF_H
#define TIMER_DRIVER_INTF_H

#include "../../OpenEDOS/kernel/include/interface.h"

class Timer_Driver_interface: public Interface_c
{
public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Services ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

    /* SID_Start_Timer */

    /**
     * @brief Send request for service: Start_Timer
     *
     * Start the timer. The timer will run endlessly and create
     * timer tick events at a fixed intervall.
     *
     * Response expected: False
     *
     * @return Error_t An error is returned if
     * - sending the message results in an error.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t sendRequest_Start_Timer(void);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Events ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

    /* EID_Timer_Tick */

    /**
     * @brief Send event: Timer_Tick
     *
     * A timer tick.
     * 

     * @return Error_t An error is returned if
     * - sending the event message results in an error.
     * Otherwise ERROR_NONE is returned.
     */
    Error_t sendEvent_Timer_Tick(void);
};

extern Timer_Driver_interface Timer_Driver_intf;

#endif//TIMER_DRIVER_INTF_H

