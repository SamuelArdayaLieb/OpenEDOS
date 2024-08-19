/**
 * OpenEDOS, (c) 2022-2024 Samuel Ardaya-Lieb, MIT License
 * 
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 */

#ifndef OE_PORT_H
#define OE_PORT_H

/**
 * @brief In this template file the hardware/RTOS dependent macros are located.
 * In order to port the kernel to a hardware platform or RTOS, suitable functions
 * must be provided here.
 */

/* Include necessary headers. */

/* No operation. May be omitted or optimizable. */
#define OE_NOP()

/**
 * Idle, e.g. some sort of low power mode. This operating mode
 * MUST be interruptable by ISRs. If such an option is not available, define
 * it as NOP().
 *
 * @param KernelID The ID of the kernel that calls this macro.
 * Only needed when there are more than one kernels.
 */
#define OE_IDLE(KernelID) 

/**
 * The kernel uses this macro to return from idle state.
 * The given function should match the OE_IDLE() macro.
 *
 * @param KernelID The ID of the kernel that will leave the idle state.
 * Only needed when there are more than one kernels.
 */
#define OE_RESUME(KernelID) 

/* Enter a section of the programm that must not be interrupted. */
#define OE_ENTER_CRITICAL() 

/* Exit a section of the programm that must not be interrupted. */
#define OE_EXIT_CRITICAL() 

#endif // OE_PORT_H