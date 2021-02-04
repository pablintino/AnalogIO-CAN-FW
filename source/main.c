
#include "main.h"


static OS_TCB AppTaskStartTCB;
static CPU_STK AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB       AppTaskObj0TCB;
static  CPU_STK      AppTaskObj0Stk[APP_CFG_TASK_OBJ_STK_SIZE];

static uint8_t aRxBuffer[2];
static uint8_t aTxBuffer[2];
static void AppTaskStart(void *p_arg);


static  void  AppTaskObj0 (void  *p_arg)
{
    OS_ERR  err;

    (void)p_arg;

    aTxBuffer[0] = 0x0F;
    aTxBuffer[1] = 0;

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

    BSP_init();


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
}
