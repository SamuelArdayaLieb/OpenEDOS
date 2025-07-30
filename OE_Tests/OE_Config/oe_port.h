/**
 * OpenEDOS, (c) 2022-2025 Samuel Ardaya-Lieb, MIT License
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
#include <stdint.h>
#include <pthread.h>
#include "oe_config.h"

/* Mutexes and conds for thread idling, declared in main.c. */
extern pthread_mutex_t condition_mutexes[OE_NUMBER_OF_KERNELS];
extern pthread_cond_t condition_conds[OE_NUMBER_OF_KERNELS];

/* Mutex for critical sections. */
extern pthread_mutex_t critical_section_mutex;

// extern atomic_bool kernel_running[OE_NUMBER_OF_KERNELS];

/* No operation. May be omitted or optimizable. */
#define OE_NOP()

static inline void __IDLE(uint8_t KernelID)
{        
    pthread_mutex_lock(&condition_mutexes[KernelID]);
    pthread_cond_wait(&condition_conds[KernelID], &condition_mutexes[KernelID]); 
    pthread_mutex_unlock(&condition_mutexes[KernelID]);
}

/**
 * Idle, e.g. some sort of low power mode. This operating mode
 * MUST be interruptable by ISRs. If such an option is not available, define
 * it as NOP().
 *
 * @param KernelID The ID of the kernel that calls this macro.
 * Only needed when there are more than one kernels.
 */
#define OE_IDLE(KernelID) __IDLE(KernelID)

static inline void __RESUME(uint8_t KernelID)
{
    pthread_mutex_lock(&condition_mutexes[KernelID]);
    pthread_cond_signal(&condition_conds[KernelID]);
    pthread_mutex_unlock(&condition_mutexes[KernelID]);
}

/**
 * The kernel uses this macro to return from idle state.
 * The given function should match the OE_IDLE() macro.
 *
 * @param KernelID The ID of the kernel that will leave the idle state.
 * Only needed when there are more than one kernels.
 */
#define OE_RESUME(KernelID) __RESUME(KernelID)

/* Enter a section of the programm that must not be interrupted. */
#define OE_ENTER_CRITICAL() pthread_mutex_lock(&critical_section_mutex)

/* Exit a section of the programm that must not be interrupted. */
#define OE_EXIT_CRITICAL() pthread_mutex_unlock(&critical_section_mutex)

#endif // OE_PORT_H