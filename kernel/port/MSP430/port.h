/**
* OpenEDOS Kernel v1.0.0
* 
* Copyright (c) 2022 Samuel Ardaya-Lieb
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
 * In this file the hardware dependent macros can be found. The correct port is
 * included by defining the corresponding PORT macro in the OpenEDOSConfig file.
 */

#ifndef PORT_H
#define PORT_H

/* Include hardware specific headers here */
#include <msp430.h>

/* No operation */
#define NOP() __no_operation()

/** 
 * Sleep. This should enable some sort of low power mode. This operating mode
 * MUST be interruptable by ISRs. If such an option is not available, define
 * it as NOP().
 */
#define SLEEP() __bis_SR_register(LPM3_bits + GIE) // Enter LPM3 w/ interrupts

/**
 * Wake up. This should end the low power mode. If the SLEEP() macro is set to 
 * NOP(), WAKE_UP() also should be defined as NOP().
 */
#define WAKE_UP() LPM3_EXIT // Exit LPM3

/* Disable interrupts */
#define DISABLE_INTERRUPTS() __disable_interrupt()

/* Enable interrupts */
#define ENABLE_INTERRUPTS() __enable_interrupt()

#endif