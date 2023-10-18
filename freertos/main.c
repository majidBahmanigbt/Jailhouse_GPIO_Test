/*
 * Copyright 2021 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "board.h"
#if __has_include("clock_config.h")
#include "clock_config.h"
#endif

#include "hlog.h"
// #include "irq.h"
// #include "os/irq.h"

// #include "pin_mux.h"
// #include "clock_config.h"
#include "fsl_iomuxc.h"
// #include "defines.h"

#define TEST_OUTPUT_GPIO GPIO5
#define TEST_OUTPUT_PIN  9U  

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Task priorities. */
#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)
#define tictac_task_PRIORITY (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
// static void hello_func(void *pvParameters);
static void tictac_func(void *pvParameters);
// extern void hello_world_doNothing(void);

/*******************************************************************************
 * Code
 ******************************************************************************/
__WEAK void BOARD_InitClocks(void) {}

/*!
 * @brief Application entry point.
 */
int main(void)
{
    BaseType_t xResult;
    
    /* Init board cpu and hardware. */
    BOARD_InitMemory();
    BOARD_InitClocks();
    BOARD_InitDebugConsole();

    // xResult = xTaskCreate(hello_func, "Hello_task", configMINIMAL_STACK_SIZE + 100, NULL, hello_task_PRIORITY, NULL);
    // assert(xResult == pdPASS);

    xResult = xTaskCreate(tictac_func, "Tictac_task", configMINIMAL_STACK_SIZE + 100, NULL, tictac_task_PRIORITY, NULL);
    assert(xResult == pdPASS);

    vTaskStartScheduler();
    for (;;)
        ;
    return xResult;
}

// /*!
//  * @brief function responsible for printing of "Hello world." message.
//  */
// static void hello_func(void *pvParameters)
// {
//     for (;;)
//     {
//         log_raw(INFO, "\r\n");
//         log_info("Hello world.\n");

//         hello_world_doNothing();

//         vTaskSuspend(NULL);
//     }
// }

/*!
 * @brief function responsible for printing of "tic tac" messages.
 */
static void tictac_func(void *pvParameters)
{
    unsigned long long count = 0;
#define TIME_DELAY_SLEEP      (1 * configTICK_RATE_HZ)

    log_raw(INFO, "START: ");

    IOMUXC_SetPinMux(IOMUXC_ECSPI1_SS0_GPIO5_IO09, 0U); 
    // IOMUXC_SetPinConfig(IOMUXC_ECSPI1_SS0_GPIO5_IO09, 
    //                     IOMUXC_SW_PAD_CTL_PAD_DSE(3U) |
    //                     IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
    //                     IOMUXC_SW_PAD_CTL_PAD_PE_MASK);
    IOMUXC_SetPinConfig(IOMUXC_ECSPI1_SS0_GPIO5_IO09,
	                    IOMUXC_SW_PAD_CTL_PAD_PUE_MASK |
	                    IOMUXC_SW_PAD_CTL_PAD_PE_MASK);  
    gpio_pin_config_t output_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
    GPIO_PinInit(TEST_OUTPUT_GPIO , TEST_OUTPUT_PIN , &output_config);

    for (;;)
    {
        GPIO_WritePinOutput(TEST_OUTPUT_GPIO, TEST_OUTPUT_PIN, 0);    
        // // GPIO5->DR = 0u;
        // SDK_DelayAtLeastUs(10000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY * 2);
        // GPIO_WritePinOutput(TEST_OUTPUT_GPIO, TEST_OUTPUT_PIN, 1);  
        // // GPIO5->DR = 1u << 9;
        // SDK_DelayAtLeastUs(30000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY * 2);
        vTaskDelay(TIME_DELAY_SLEEP);

        if (++count % 2)
            log_raw(INFO, "tic1 ");
        else
            log_raw(INFO, "tac2 ");

        if (!(count % 20))
            log_raw(INFO, "\r\n");
    }
}