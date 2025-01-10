/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#include "main.h"
#include "build_defs.h"
#include "version_numbers.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include <SEGGER_RTT.h>

static StaticTask_t AppTaskObj0TCB;
static StackType_t AppTaskObj0Stk[APP_CFG_TASK_OBJ_STK_SIZE];

static StaticTask_t AppTaskCANTCB;
static StackType_t AppTaskCANStk[APP_CFG_TASK_OBJ_STK_SIZE];

static SemaphoreHandle_t adc_sync_sem;
static StaticSemaphore_t adc_sync_sem_buffer;

static uint8_t aRxBuffer[2];
static uint8_t aTxBuffer[2];

static uint16_t adc_dma_conversions[2];

const unsigned char completeVersion[] = {VERSION_MAJOR_INIT,
                                         '.',
                                         VERSION_MINOR_INIT,
                                         '-',
                                         'V',
                                         '-',
                                         BUILD_YEAR_CH0,
                                         BUILD_YEAR_CH1,
                                         BUILD_YEAR_CH2,
                                         BUILD_YEAR_CH3,
                                         '-',
                                         BUILD_MONTH_CH0,
                                         BUILD_MONTH_CH1,
                                         '-',
                                         BUILD_DAY_CH0,
                                         BUILD_DAY_CH1,
                                         'T',
                                         BUILD_HOUR_CH0,
                                         BUILD_HOUR_CH1,
                                         ':',
                                         BUILD_MIN_CH0,
                                         BUILD_MIN_CH1,
                                         ':',
                                         BUILD_SEC_CH0,
                                         BUILD_SEC_CH1,
                                         '\0'};
void SysTick_Handler(void)
{
    xPortSysTickHandler();
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

uint32_t test_n = 0;
static void AppTaskObj0(void *p_arg)
{
    (void)p_arg;

    aTxBuffer[0] = 0x0F;
    aTxBuffer[1] = 0;

    SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");

    bio_write_port(GPIOA, 5, 1);
    vTaskDelay(500);
    // i2c_transfer7(I2C3, 0x90U, &aTxBuffer, 1, &aRxBuffer, 2);

    bi2c_master_transfer(I2C3, 0x90U, aTxBuffer, 1, true, 500);
    bi2c_master_transfer(I2C3, 0x90U, aRxBuffer, 2, false, 500);
    vTaskDelay(500);

    /*
        BSP_USART_put_char(USART1, 'T', 100U);
        BSP_USART_put_char(USART1, 'E', 100U);
        BSP_USART_put_char(USART1, 'S', 100U);
        BSP_USART_put_char(USART1, 'T', 100U);
        BSP_USART_put_char(USART1, '\r', 100U);
        busart_put_char(USART1, '\n', 100U);
    */
    /*for (int i =0 ; i < 5;i++) {

        BSP_USART_put_char(USART1, 'H', 100U);
        BSP_IO_toggle_pin(GPIOA, 5);
        OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &err);
        busart_put_char(USART1, 'O', 100U);
        bio_toggle_port(GPIOA, 5);
        OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &err);
    }*/

    for (;;) {
        if (aRxBuffer[0] == 0x75U && aRxBuffer[1] == 0x00U) {
            // busart_put_char(USART1, 'H', 100U);
            bio_toggle_port(GPIOA, 4);
            vTaskDelay(1000);
        } else {
            bio_toggle_port(GPIOA, 6);
            vTaskDelay(1000);
        }
    }
}

static void AppTaskCanTX(void *p_arg)
{
    (void)p_arg;

    bcan_tx_metadata_t test;
    test.id = 0x7ff;
    test.is_rtr = false;
    test.size_b = 4;
    test.store_tx_events = false;
    test.message_marker = 0x00;

    for (;;) {
        vTaskDelay(500);

        badc_start_conversion_dma(ADC1, DMA1, BDMA_CHANNEL_1, (uint8_t *)&adc_dma_conversions, 2);

        xSemaphoreTake(adc_sync_sem, portMAX_DELAY);
        uint32_t conversion_value = adc_dma_conversions[0] | (adc_dma_conversions[1] << 16);
        if (bcan_add_tx_message(FDCAN1, &test, (const uint8_t *)&conversion_value) != STATUS_OK) {
            SEGGER_RTT_WriteString(0, "SEND ERRRRRR\r\n");
        }
    }
}

void can_rx_handler(bcan_instance_t *can, uint32_t group_flags)
{
    (void)group_flags;

    bcan_rx_metadata_t rx_metadata;
    uint8_t rx_data[64];
    if (bcan_get_rx_message(can, BCAN_RX_QUEUE_O, &rx_metadata, rx_data) == STATUS_OK) {
        test_n++;
    }
}

void adc_eos_handler(badc_instance_t *adc, uint32_t flags)
{
    (void)adc;
    (void)flags;

    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(adc_sync_sem, &xHigherPriorityTaskWoken);
}

void dma_xfer_complete_handler(bdma_instance_t *dma, bdma_channel_instance_t *channel, uint32_t group_flags)
{
    (void)dma;
    (void)channel;
    (void)group_flags;

    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(adc_sync_sem, &xHigherPriorityTaskWoken);
}

int main(void)
{
    SEGGER_RTT_printf(0, "### Analog-IO SW Version %s@pablintino ###\r\n", completeVersion);

    board_init();
    bcan_config_irq(FDCAN1, BCAN_IRQ_TYPE_RF0NE, can_rx_handler);
    // badc_config_irq(ADC1, BADC_ISR_TYPE_EOS, adc_eos_handler);

    bdma_config_irq(DMA1, BDMA_CHANNEL_1, BDMA_ISR_TYPE_XFER_COMPL, dma_xfer_complete_handler);

    /* -2- Configure IO in output push-pull mode to drive external LEDs */
    bio_conf_output_port(GPIOA, BSP_IO_PIN_4 | BSP_IO_PIN_5 | BSP_IO_PIN_6, BSP_IO_PU, BSP_IO_HIGH, BSP_IO_OUT_TYPE_PP);

    adc_sync_sem = xSemaphoreCreateBinaryStatic(&adc_sync_sem_buffer);

    xTaskCreateStatic(AppTaskObj0,
                      "Kernel Objects Task 0",
                      APP_CFG_TASK_OBJ_STK_SIZE,
                      NULL,
                      APP_CFG_TASK_OBJ_PRIO,
                      AppTaskObj0Stk,
                      &AppTaskObj0TCB);
    xTaskCreateStatic(AppTaskCanTX,
                      "CAN Task 0",
                      APP_CFG_TASK_OBJ_STK_SIZE,
                      NULL,
                      APP_CFG_TASK_OBJ_PRIO,
                      AppTaskCANStk,
                      &AppTaskCANTCB);

    vTaskStartScheduler();

    for (;;)
        ; /* Should Never Get Here.                               */
}
