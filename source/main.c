
#include "main.h"


static  OS_TCB          AppTaskStartTCB;
static  CPU_STK         AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static  void         AppTaskStart       (void  *p_arg);

int main(void) {
    OS_ERR os_err;


    CPU_IntDis();                                               /* Disable all Interrupts.                              */
    CPU_Init();

    OSInit(&os_err);
    if(os_err != OS_ERR_NONE){
        while(1);
    }

    OSTaskCreate(&AppTaskStartTCB,                              /* Create the start task                                */
                 "Start Task",
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


static  void  AppTaskStart (void *p_arg)
{
    (void)p_arg;

    OS_ERR  err;
    GPIO_InitTypeDef  GPIO_InitStruct;

    BSP_configure_clocks();


    LED3_GPIO_CLK_ENABLE();

    /* -2- Configure IO in output push-pull mode to drive external LEDs */
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_InitStruct.Pin = LED3_PIN;
    HAL_GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStruct);



    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        HAL_GPIO_TogglePin(LED3_GPIO_PORT, LED3_PIN);
        OSTimeDly(100,
                  OS_OPT_TIME_PERIODIC,
                  &err);

    }
}
