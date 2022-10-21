/**
* OpenEDOS Kernel v1.2.0
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

#include <stdint.h>

/* Needed for NUMBER_OF_KERNELS and KernelID, necessary for the FreeRTOS adapter */
#include "../OpenEDOS/OpenEDOSConfig.h"
#include "../OpenEDOS/kernel/include/defines.h"

/* Include hardware specific headers here */
#include <FreeRTOS.h>
#include <portmacro.h>
#include <task.h>

/* FreeRTOS adapter, declared in main.cpp */
extern TaskHandle_t TaskHandles[NUMBER_OF_KERNELS];

/* No operation */
#define NOP() portNOP() //__asm volatile("NOP");

/** 
 * Idle, e.g. some sort of low power mode. This operating mode
 * MUST be interruptable by ISRs. If such an option is not available, define
 * it as NOP().
 */

/*
// Enter low power mode directly.
static inline __attribute__((always_inline)) void __IDLE(void)
{
    __asm volatile( "dsb" );
    __asm volatile( "wfi" );
    __asm volatile( "isb" );
}
*/

static inline __attribute__((always_inline)) void __IDLE(uint8_t KernelID)
{
    vTaskSuspend(TaskHandles[KernelID]);
}

#define IDLE(KernelID) __IDLE(KernelID)

static inline __attribute__((always_inline)) void __RESUME(uint8_t KernelID)
{
    vTaskResume(TaskHandles[KernelID]);
}

#define RESUME(KernelID) __RESUME(KernelID)

/*disable all interrupts*/
static inline __attribute__((always_inline)) void __DISABLE_IRQ(void)
{
    __asm volatile("cpsid i" ::: "memory");
}

/*enable all interrupts*/
static inline __attribute__((always_inline)) void __ENABLE_IRQ(void)
{
    __asm volatile("cpsie i" ::: "memory");
}

/* Disable interrupts */
#define ENTER_CRITICAL() portENTER_CRITICAL() //__DISABLE_IRQ()

/* Enable interrupts */
#define EXIT_CRITICAL() portEXIT_CRITICAL() //__ENABLE_IRQ()

#endif
