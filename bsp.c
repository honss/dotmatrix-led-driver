#include <stdint.h>  /* Standard integers. WG14/N843 C99 Standard */
#include "bsp.h"


static uint32_t volatile l_tickCtr;
extern int DELAY;
int counter = 0;

void BSP_init(void) {

    CMU_ClockEnable(cmuClock_CORELE, true);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
    CMU->HFPERCLKEN0 = (1U << 7) | (1U);
    GPIO->CTRL |= 2;
    
    //GPIO->P[2].MODEH |= (1<<10);
    //GPIO->P[2].MODEH |= (1<<30); //15 clk
    //GPIO->P[3].MODEL |= (1<<30); //7

    //GPIO->P[2].DOUTSET |= (1<<10|1<<11);
    GPIO->P[4].MODEH |= (1<<10) | (3<<12) | (1<<18) | (3<<20); //PE10 OUTPUT, PE11 INPUT PE 12 OUT, PE 13 IN
    GPIO->P[4].DOUTSET |= (1<<10) | (1<<12);
    GPIO->EXTIPSELH |= (1<<14) | (1<<22);
    GPIO->EXTIRISE |= (1<<11);
    GPIO->EXTIFALL |= (1<<13);
    GPIO->IEN |= (1<<11) | (1<<13);
    NVIC_EnableIRQ(GPIO_ODD_IRQn);

   
    //SystemCoreClockUpdate();
    //SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);
      CMU_ClockEnable(cmuClock_HFLE, true); 
    //CMU_ClockSelectSet(cmuClock_RTC, cmuSelect_LFXO);
    CMU->LFACLKEN0 |= 1;
    CMU->OSCENCMD |= (1<<6);
    CMU->LFCLKSEL |= 1;
    RTC->CTRL |= (1 | 1<<1);
    //RTC->IEN |= (1<<1);
    //RTC->COMP0 = 320;
    CMU_ClockEnable(cmuClock_PCNT0, true);
    //NVIC_EnableIRQ(PCNT0_IRQn);
    //CMU->HFPERCLKEN0 |= (1 << 3);
    /* Initialize with default settings and then update fields according to application requirements. */
    /*USART1->CTRL |= (1);
    USART1->FRAME |= 13;
    USART1->CMD |= (1<<2);
    USART1->ROUTE |= 0x30A;
    USART1->TXDOUBLE |= 0xFF;*/

    
}



uint32_t BSP_tickCtr(void) {
    uint32_t tickCtr;

    __disable_irq();
    tickCtr = RTC->CNT;
    __enable_irq();

    return tickCtr;
}


void SysTick_Handler(void) {
  while(1) {}
}

void GPIO_ODD_IRQHandler(void) {
  if(GPIO->IF&(1<<13)) {
    counter = RTC->CNT;
  }
  if(GPIO->IF&(1<<11)) {
    number = RTC->CNT - counter;
    updateMatrix(((float)number)/32000,matrix);
  }
  GPIO->IFC = (1<<11) | (1<<13);
}

void BSP_delay(uint32_t ticks) {
    uint32_t start = BSP_tickCtr();
    while (((BSP_tickCtr() - start)) < ticks) {
    }
}

void BSP_setLED(void) {
  GPIO->P[2].DOUTSET |= (1<<10);
}

void BSP_clearLED(void) {
  GPIO->P[2].DOUTCLR |= (1<<10);
}
void BSP_setLED2(void) {
  GPIO->P[2].DOUTSET |= (1<<11);
}

void BSP_clearLED2(void) {
  GPIO->P[2].DOUTCLR |= (1<<11);
}