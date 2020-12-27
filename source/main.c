
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


static  void  AppTaskStart (void *p_arg)
{
    (void)p_arg;

    OS_ERR  err;

    BSP_init();



    /* -2- Configure IO in output push-pull mode to drive external LEDs */
    BSP_IO_conf_output_pin(GPIOA, 4, BSP_IO_PP, BSP_IO_HIGH);
    BSP_IO_conf_output_pin(GPIOA, 5, BSP_IO_PP, BSP_IO_HIGH);
    BSP_IO_conf_output_pin(GPIOA, 6, BSP_IO_PP, BSP_IO_HIGH);
    //BSP_IO_conf_input_pin(GPIOB, 3, BSP_IO_PP, BSP_IO_HIGH);


    while (DEF_TRUE) {                                /* Task body, always written as an infinite loop. */

        OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &err);

        BSP_IO_write_pin(GPIOA, 4, 1);
        OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &err);
        BSP_IO_write_pin(GPIOA, 5, 1);
        OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &err);
        BSP_IO_write_pin(GPIOA, 6, 1);

        OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &err);

        BSP_IO_write_pin(GPIOA, 4, 0);
        OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &err);
        BSP_IO_write_pin(GPIOA, 5, 0);
        OSTimeDly(1000, OS_OPT_TIME_PERIODIC, &err);
        BSP_IO_write_pin(GPIOA, 6, 0);
        /*if(BSP_IO_read_pin(GPIOA, 4) == 1){
            BSP_write_pin(GPIOA, 3, 1);
        }else{
            BSP_IO_write_pin(GPIOA, 3, 0);
        }*/

    }
}
