/* Copyright (C) Pablo Rodriguez Nava - All Rights Reserved
 *       * Unauthorized copying of this file, via any medium is strictly prohibited
 *       * Proprietary and confidential
 * Written by Pablo Rodriguez Nava <info@pablintino.com>, June 2021
 */

#include "main.h"
#include "build_defs.h"
#include "version_numbers.h"

#include "tx_api.h"
#include <SEGGER_RTT.h>

static UCHAR tx_byte_pool_buffer[APP_CFG_BYTE_POOL_SIZE] __attribute__((aligned(4U)));
static TX_BYTE_POOL tx_app_byte_pool;

TX_THREAD TX_thread_adc_sync;
TX_THREAD TX_thread_0;
TX_THREAD TX_thread_start;
TX_SEMAPHORE TX_adc_sync_sem;

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

uint32_t test_n = 0;
static void AppTaskObj0(ULONG p_arg)
{
    (void)p_arg;

    aTxBuffer[0] = 0x0F;
    aTxBuffer[1] = 0;

    // SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");

    bio_write_port(GPIOA, 5, 1);
    //  i2c_transfer7(I2C3, 0x90U, &aTxBuffer, 1, &aRxBuffer, 2);
    ret_status status1 = bi2c_master_transfer(I2C3, 0x90U, aTxBuffer, 1, true, 1000);
    if (status1 != STATUS_OK) {
        for (;;) {

            bio_toggle_port(GPIOA, 6);
            tx_thread_sleep(5000);
        }
    }
    ret_status status2 = bi2c_master_transfer(I2C3, 0x90U, aRxBuffer, 2, false, 1000);
    if (status2 != STATUS_OK) {
        for (;;) {

            bio_toggle_port(GPIOA, 6);
            tx_thread_sleep(5000);
        }
    }

    busart_put_char(USART1, 'T', 100U);
    busart_put_char(USART1, 'E', 100U);
    busart_put_char(USART1, 'S', 100U);
    busart_put_char(USART1, 'T', 100U);
    busart_put_char(USART1, '\n', 100U);

    for (;;) {
        if (aRxBuffer[0] == 0x75U && aRxBuffer[1] == 0x00U) {
            // busart_put_char(USART1, 'H', 100U);
            bio_toggle_port(GPIOA, 4);
            tx_thread_sleep(1000);
        } else {
            bio_toggle_port(GPIOA, 6);
            tx_thread_sleep(500);
        }
    }
}

static void AppTaskCanTX(ULONG p_arg)
{
    (void)p_arg;

    bcan_tx_metadata_t test;
    test.id = 0x7ff;
    test.is_rtr = false;
    test.size_b = 4;
    test.store_tx_events = false;
    test.message_marker = 0x00;

    for (;;) {
        tx_thread_sleep(500);

        badc_start_conversion_dma(ADC1, DMA1, BDMA_CHANNEL_1, (uint8_t *)&adc_dma_conversions, 2);

        tx_semaphore_get(&TX_adc_sync_sem, TX_WAIT_FOREVER);
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

    tx_semaphore_put(&TX_adc_sync_sem);
}

void dma_xfer_complete_handler(bdma_instance_t *dma, bdma_channel_instance_t *channel, uint32_t group_flags)
{
    (void)dma;
    (void)channel;
    (void)group_flags;

    tx_semaphore_put(&TX_adc_sync_sem);
}

static void AppStart(ULONG p_arg)
{
    (void)p_arg;

    tx_thread_sleep(100);
    board_init();
    bcan_config_irq(FDCAN1, BCAN_IRQ_TYPE_RF0NE, can_rx_handler);
    // badc_config_irq(ADC1, BADC_ISR_TYPE_EOS, adc_eos_handler);

    bdma_config_irq(DMA1, BDMA_CHANNEL_1, BDMA_ISR_TYPE_XFER_COMPL, dma_xfer_complete_handler);

    /* -2- Configure IO in output push-pull mode to drive external LEDs */
    bio_conf_output_port(GPIOA, BSP_IO_PIN_4 | BSP_IO_PIN_5 | BSP_IO_PIN_6, BSP_IO_PU, BSP_IO_HIGH, BSP_IO_OUT_TYPE_PP);

    if (tx_semaphore_create(&TX_adc_sync_sem, "adc sync sem", 0) != TX_SUCCESS) {
        for (;;)
            ;
    }
    if (tx_semaphore_ceiling_put(&TX_adc_sync_sem, 1) != TX_SUCCESS) {
        for (;;)
            ;
    }

    char *stack_can_tx_thread;
    if (tx_byte_allocate(&tx_app_byte_pool, (void **)&stack_can_tx_thread, APP_CFG_TASK_OBJ_STK_SIZE, TX_NO_WAIT) !=
        TX_SUCCESS) {
        for (;;)
            ;
    }
    if (tx_thread_create(&TX_thread_adc_sync,
                         "CAN Task",
                         AppTaskCanTX,
                         0,
                         stack_can_tx_thread,
                         APP_CFG_TASK_OBJ_STK_SIZE,
                         APP_CFG_TASK_OBJ_PRIO,
                         APP_CFG_TASK_OBJ_PRIO,
                         TX_NO_TIME_SLICE,
                         TX_AUTO_START) != TX_SUCCESS) {
        for (;;)
            ;
    }

    char *stack_app0_thread;
    if (tx_byte_allocate(&tx_app_byte_pool, (void **)&stack_app0_thread, APP_CFG_TASK_OBJ_STK_SIZE, TX_NO_WAIT) !=
        TX_SUCCESS) {
        for (;;)
            ;
    }
    if (tx_thread_create(&TX_thread_0,
                         "Task 0",
                         AppTaskObj0,
                         0,
                         stack_app0_thread,
                         APP_CFG_TASK_OBJ_STK_SIZE,
                         APP_CFG_TASK_OBJ_PRIO - 1,
                         APP_CFG_TASK_OBJ_PRIO - 1,
                         TX_NO_TIME_SLICE,
                         TX_AUTO_START) != TX_SUCCESS) {
        for (;;)
            ;
    }
}
void tx_application_define(VOID *first_unused_memory)
{
    (void)first_unused_memory;

    if (tx_byte_pool_create(&tx_app_byte_pool, "Tx App memory pool", tx_byte_pool_buffer, APP_CFG_BYTE_POOL_SIZE) !=
        TX_SUCCESS) {
        for (;;)
            ;
    }

    char *stack_start_thread;
    if (tx_byte_allocate(&tx_app_byte_pool, (void **)&stack_start_thread, APP_CFG_TASK_OBJ_STK_SIZE, TX_NO_WAIT) !=
        TX_SUCCESS) {
        for (;;)
            ;
    }
    if (tx_thread_create(&TX_thread_start,
                         "Task 0",
                         AppStart,
                         0,
                         stack_start_thread,
                         APP_CFG_TASK_OBJ_STK_SIZE,
                         APP_CFG_TASK_OBJ_PRIO - 2,
                         APP_CFG_TASK_OBJ_PRIO - 2,
                         TX_NO_TIME_SLICE,
                         TX_AUTO_START) != TX_SUCCESS) {
        for (;;)
            ;
    }
}

int main(void)
{
    SEGGER_RTT_printf(0, "### Analog-IO SW Version %s@pablintino ###\r\n", completeVersion);

    board_early_init();
    tx_kernel_enter();

    for (;;)
        ; /* Should Never Get Here.                               */
}
