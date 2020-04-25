#include "bsp_clocks.h"

void bsp_clocks_configure(void){
    
    // Configure SysTick to generate an IRQ event every 1ms
    SysTick_Config(64000UL);

}
