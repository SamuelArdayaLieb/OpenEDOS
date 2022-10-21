/*
 * main.cpp
 *
 *  Created on: 25.08.2022
 *      Author: samuel
 */

#include <TM4C123GH6PM.h>

#include "driverlib/sysctl.h"

#include <FreeRTOS.h>
#include <task.h>

#include "OpenEDOS/kernel/include/kernel.h"
#include "OpenEDOS/kernel/include/kernel_switch.h"

#include "Modules/Timer_Driver/timer_driver_intf.h"
#include "Modules/Timer_Driver/timer_driver_mod.h"

#include "Modules/GPIO_Driver/gpio_driver_intf.h"
#include "Modules/GPIO_Driver/gpio_driver_mod.h"

#include "Modules/LED_Handler/led_handler_mod.h"

void sys_clk_set();

void GPIO_task(void *args);
void Timer_task(void *args);
void LED_task(void *args);

static void init_systick()
{
    /* We set OS tick to 10 ms.
     */
    int ms = 10;

    unsigned long clk = SysCtlClockGet() / 1000;

    SysTick->CTRL = 0;
    SysTick->LOAD = ms * clk - 1;
    SysTick->CTRL = (1 << 1) | (1 << 2);
    SysTick->CTRL |= 1;
}

/* We need one FreeRTOS TaskHandle for each OpenEDOS Kernel. */
TaskHandle_t TaskHandles[NUMBER_OF_KERNELS];

KernelSwitch_c KernelSwitch;

int main()
{
    portDISABLE_INTERRUPTS();

    sys_clk_set();

    KernelSwitch.init();

    GPIO_Driver_intf.connect(&KernelSwitch);
    Timer_Driver_intf.connect(&KernelSwitch);

    xTaskCreate(GPIO_task, "GPIO", 256, &KernelSwitch, 2, &TaskHandles[0]);
    xTaskCreate(Timer_task, "Timer", 256, &KernelSwitch, 2, &TaskHandles[1]);
    xTaskCreate(LED_task, "LED", 256, &KernelSwitch, 2, &TaskHandles[2]);

    init_systick();

    portENABLE_INTERRUPTS();

    vTaskStartScheduler();

    return 0;
}

void sys_clk_set()
{
    volatile int i;

    SYSCTL->RCC |= (1<<11);     /* Enable BYPASS */
    SYSCTL->RCC2 |= (1 << 11); /* Enable BYPASS2 */

    SYSCTL->RCC &= ~(1<<22); /* Clr USESYSDIV */

    for (i = 0; i < 10000; i++) {
        __asm volatile ("nop");
    }

    /* Enable OSC */
    SYSCTL->RCC &= ~(1<<0); //MOSCDIS

    while ((SYSCTL->RIS & (1 << 8)) == 0) {
        __asm volatile ("nop");
    }

    SYSCTL->RCC &= ~(0x1f << 6); //CLR XTAL
    SYSCTL->RCC |= (0x15 << 6); //XTAL
    SYSCTL->RCC &= ~(3 << 4); //OSCSRC
    SYSCTL->RCC2 &= ~(7 << 4); //OSCSRC2

    SYSCTL->RCC &= ~(1 << 13); //PWRDN
    SYSCTL->RCC2 &= ~(1 << 13); /* Clr PWRDN2 */

    /* Select SYSDIV2 */

    /* Use RCC2 */
    SYSCTL->RCC2 |= (1 << 31);

    SYSCTL->RCC2 |= (1 << 30);      /* DIV400 */
    SYSCTL->RCC2 &= ~(63 << 23);    /* Clr SYSDIV2 */
    SYSCTL->RCC2 &= ~(1 << 22);     /* Clr SYSDIV2LSB */
    SYSCTL->RCC2 |= (2 << 23);      /* Set SYSDIV2 */
    /* SYSCTL->RCC2 |= (1 << 22); */        /* Set SYSDIV2LSB */

    SYSCTL->RCC |= (1<<22);         /* Re-enable USESYSDIV */

    /* Wait for PLL lock by polling PLLLRIS in RIS */
    while ((SYSCTL->RIS & (1 << 6)) == 0) {
        __asm volatile ("nop");
    }

    /* Clear BYPASS */
    SYSCTL->RCC2 &= ~(1 << 11);
    SYSCTL->RCC &= ~(1 << 11);

    /* Let clock stabilize */
    for (int i = 0; i < 1000; i++){
        __asm volatile ("nop");
    }
}

void GPIO_task(void *args)
{
    Kernel_c GPIO_Kernel;
    GPIO_Driver_module GPIO_Driver;
    KernelSwitch_c *Switch = (KernelSwitch_c*)args;

    GPIO_Kernel.init();

    Switch->connectKernel(&GPIO_Kernel);

    GPIO_Kernel.connectModule(&GPIO_Driver);

    GPIO_Kernel.initModules();

    GPIO_Kernel.start();

    GPIO_Kernel.run();
}

void Timer_task(void *args)
{
    Kernel_c Timer_Kernel;
    Timer_Driver_module Timer_Driver;
    KernelSwitch_c *Switch = (KernelSwitch_c*)args;

    Timer_Kernel.init();

    Switch->connectKernel(&Timer_Kernel);

    Timer_Kernel.connectModule(&Timer_Driver);

    Timer_Kernel.initModules();

    Timer_Kernel.start();

    Timer_Kernel.run();
}

void LED_task(void *args)
{
    Kernel_c LED_Kernel;
    LED_Handler_module LED_Handler;
    KernelSwitch_c *Switch = (KernelSwitch_c*)args;

    LED_Kernel.init();

    Switch->connectKernel(&LED_Kernel);

    LED_Kernel.connectModule(&LED_Handler);

    LED_Kernel.initModules();

    LED_Kernel.start();

    LED_Kernel.run();
}
