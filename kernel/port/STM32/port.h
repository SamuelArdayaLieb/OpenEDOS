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
 * In this template file the hardware dependent macros can be found. If you want 
 * to port the kernel to a new hardware platform simply provide the macros below 
 * and include the created port header.
 */

#ifndef PORT_H
#define PORT_H

/* Include hardware specific headers here */
#include "stm32l4xx_hal.h"
#include <stdint.h>

typedef uint32_t BaseType_t;

/* No operation */
#define NOP() __asm volatile("NOP");

/** 
 * Sleep. This should enable some sort of low power mode. This operating mode
 * MUST be interruptable by ISRs. If such an option is not available, define
 * it as NOP().
 */
static inline __attribute__((always_inline)) void __SLEEP(void)
{
	HAL_SuspendTick();

	HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

#define SLEEP() __SLEEP()

/**
 * Wake up. This should end the low power mode. If the SLEEP() macro is set to 
 * NOP(), WAKE_UP() also should be defined as NOP().
 */
static inline __attribute__((always_inline)) void __WAKE_UP(void)
{
	HAL_ResumeTick();
}

#define WAKE_UP() __WAKE_UP()

/*disable all interrupts*/
static inline __attribute__((always_inline)) void __DISABLE_IRQ(void)
{
	__asm volatile("CPSID i");
}

/*enable all interrupts*/
static inline __attribute__((always_inline)) void __ENABLE_IRQ(void)
{
	__asm volatile("CPSIE i");
}


/* Disable interrupts */
#define DISABLE_INTERRUPTS() __DISABLE_IRQ()

/* Enable interrupts */
#define ENABLE_INTERRUPTS() __ENABLE_IRQ()

#endif