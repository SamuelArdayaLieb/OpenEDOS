/**
 * OpenEDOS Core v0.2.1
 *
 * Copyright (c) 2022-2024 Samuel Ardaya-Lieb
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * https://github.com/SamuelArdayaLieb/OpenEDOS
 *
 */

/**
 * In this template file the hardware/RTOS dependent macros are located.
 * In order to port the kernel to a hardware platform or RTOS, suitable functions
 * must be provided here.
 */

#ifndef PORT_H
#define PORT_H

/* Include necessary headers. */

/* No operation. May be omitted/optimizable. */
#define NOP()

/**
 * Idle, e.g. some sort of low power mode. This operating mode
 * MUST be interruptable by ISRs. If such an option is not available, define
 * it as NOP().
 *
 * @param KernelID The ID of the kernel that calls this macro.
 * Only needed when there are more than one kernels.
 */
#define IDLE(KernelID) 

/**
 * The kernel uses this macro to return from idle state.
 * The given function should match the IDLE() macro.
 *
 * @param KernelID The ID of the kernel that will leave the idle state.
 * Only needed when there are more than one kernels.
 */
#define RESUME(KernelID) 

/* Enter a section of the programm that must not be interrupted. */
#define ENTER_CRITICAL() 

/* Exit a section of the programm that must not be interrupted. */
#define EXIT_CRITICAL() 

#endif // PORT_H