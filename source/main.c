
#include <SEGGER_RTT.h>
#include "main.h"
#include "version_numbers.h"
#include "build_defs.h"


static OS_TCB AppTaskStartTCB;
static CPU_STK AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB       AppTaskObj0TCB;
static  CPU_STK      AppTaskObj0Stk[APP_CFG_TASK_OBJ_STK_SIZE];
static  OS_TCB       AppTaskCANTCB;
static  CPU_STK      AppTaskCANStk[APP_CFG_TASK_OBJ_STK_SIZE];

static uint8_t aRxBuffer[2];
static uint8_t aTxBuffer[2];
static void AppTaskStart(void *p_arg);

const unsigned char completeVersion[] =
        {
                VERSION_MAJOR_INIT,
                '.',
                VERSION_MINOR_INIT,
                '-', 'V', '-',
                BUILD_YEAR_CH0, BUILD_YEAR_CH1, BUILD_YEAR_CH2, BUILD_YEAR_CH3,
                '-',
                BUILD_MONTH_CH0, BUILD_MONTH_CH1,
                '-',
                BUILD_DAY_CH0, BUILD_DAY_CH1,
                'T',
                BUILD_HOUR_CH0, BUILD_HOUR_CH1,
                ':',
                BUILD_MIN_CH0, BUILD_MIN_CH1,
                ':',
                BUILD_SEC_CH0, BUILD_SEC_CH1,
                '\0'
        };

uint32_t test_n = 0;
static  void  AppTaskObj0 (void  *p_arg)
{
    OS_ERR  err;

    (void)p_arg;

    aTxBuffer[0] = 0x0F;
    aTxBuffer[1] = 0;

    SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");

    BSP_IO_write_pin(GPIOA, 5, 1);

    OSTimeDly(500, OS_OPT_TIME_PERIODIC, &err);

    //i2c_transfer7(I2C3, 0x90U, &aTxBuffer, 1, &aRxBuffer, 2);

    BSP_I2C_master_transfer(I2C3, 0x90U, &aTxBuffer, 1, true, 500);
    BSP_I2C_master_transfer(I2C3, 0x90U, &aRxBuffer, 2, false, 500);
    OSTimeDly(500, OS_OPT_TIME_PERIODIC, &err);


/*
    BSP_USART_put_char(USART1, 'T', 100U);
    BSP_USART_put_char(USART1, 'E', 100U);
    BSP_USART_put_char(USART1, 'S', 100U);
    BSP_USART_put_char(USART1, 'T', 100U);
    BSP_USART_put_char(USART1, '\r', 100U);
    BSP_USART_put_char(USART1, '\n', 100U);
*/
    /*for (int i =0 ; i < 5;i++) {

        BSP_USART_put_char(USART1, 'H', 100U);
        BSP_IO_toggle_pin(GPIOA, 5);
        OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &err);
        BSP_USART_put_char(USART1, 'O', 100U);
        BSP_IO_toggle_pin(GPIOA, 5);
        OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &err);
    }*/

    while (DEF_TRUE) {
        if(aRxBuffer[0]==0x75U && aRxBuffer[1]==0x00U){
            //BSP_USART_put_char(USART1, 'H', 100U);
            BSP_IO_toggle_pin(GPIOA, 4);
            OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &err);
        }else{
            BSP_IO_toggle_pin(GPIOA, 6);
            OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &err);
        }

    }
}


static  void  AppTaskCanTX (void  *p_arg)
{
    OS_ERR  err;

    (void)p_arg;


    bsp_can_tx_metadata test;
    test.ID = 0x123;
    test.IsRTR = false;
    test.DataLength= 0x02;
    test.StoreTxEvents = false;
    test.MessageMarker = 0x00;
    uint8_t data[8];
    data[0] = 0x01;
    data[1] = 0x02;
    data[2] = 0x02;
    data[3] = 0x02;
    data[4] = 0x02;
    data[5] = 0x02;
    data[6] = 0x02;
    data[7] = 0x02;

    uint32_t test_tmp = test_n;
    while (DEF_TRUE) {
        OSTimeDly(10, OS_OPT_TIME_PERIODIC, &err);

        /*if(BSP_CAN_add_tx_message(FDCAN1, &test, &data) != STATUS_OK){
            SEGGER_RTT_WriteString(0, "SEND ERRRRRR\r\n");
        }*/
        if(test_n!= test_tmp){
            test_tmp = test_n;
            SEGGER_RTT_printf(0, "[INFO] >> %u\r\n", test_n);
        }

    }
}


void can_rx_handler(BSP_CAN_Instance *can, uint32_t group_flags){
    bsp_can_rx_metadata_t rx_metadata;
    uint8_t rx_data[64];
    if(BSP_CAN_get_rx_message(FDCAN1, BSP_CAN_RX_QUEUE_O, &rx_metadata, &rx_data) == STATUS_OK){
        test_n++;
    }

}




int main(void) {
    OS_ERR os_err;

    CPU_IntDis();                                               /* Disable all Interrupts.                              */
    CPU_Init();

    /* Init uC/Os micro kernel */
    OSInit(&os_err);
    if (os_err != OS_ERR_NONE) {
        while (1);
    }

    /* Launch the bootstrap task */
    OSTaskCreate(
            &AppTaskStartTCB,                              /* Create the start task                                */
            "Start Tsk",
            AppTaskStart,
            0u,
            APP_CFG_TASK_START_PRIO,
            &AppTaskStartStk[0u],
            AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
            APP_CFG_TASK_START_STK_SIZE,
            0u,
            0u,
            0u,
            (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
            &os_err);
    if (os_err != OS_ERR_NONE) {
        while (1);
    }

    OSStart(&os_err);                                           /* Start multitasking (i.e. give control to uC/OS-III). */

    while (DEF_ON) {                                            /* Should Never Get Here.                               */
        ;
    }

}


static void AppTaskStart(void *p_arg) {
    (void) p_arg;

    OS_ERR err;
    SEGGER_RTT_printf(0, "### Analog-IO SW Version %s@pablintino ###\r\n", completeVersion);
    BSP_init();


    BSP_CAN_config_irq(FDCAN1, BSP_CAN_IRQ_TYPE_RF0NE, can_rx_handler);

    /* -2- Configure IO in output push-pull mode to drive external LEDs */
    BSP_IO_conf_output_pin(GPIOA,
                           BSP_IO_PIN_4 | BSP_IO_PIN_5 | BSP_IO_PIN_6, BSP_IO_PU,
                           BSP_IO_HIGH,
                           BSP_IO_OUT_TYPE_PP);

    OSTaskCreate(&AppTaskObj0TCB,
                 "Kernel Objects Task 0",
                 AppTaskObj0,
                 0,
                 APP_CFG_TASK_OBJ_PRIO,
                 &AppTaskObj0Stk[0],
                 AppTaskObj0Stk[APP_CFG_TASK_OBJ_STK_SIZE / 10u],
                 APP_CFG_TASK_OBJ_STK_SIZE,
                 0u,
                 0u,
                 0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &err);

    OSTaskCreate(&AppTaskCANTCB,
                 "CAN Task 0",
                 AppTaskCanTX,
                 0,
                 APP_CFG_TASK_OBJ_PRIO,
                 &AppTaskCANStk[0],
                 AppTaskCANStk[APP_CFG_TASK_OBJ_STK_SIZE / 10u],
                 APP_CFG_TASK_OBJ_STK_SIZE,
                 0u,
                 0u,
                 0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &err);


}
