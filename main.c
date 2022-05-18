
#include "bsp.h"
//#include "startup_efm32.c"
//#define 	CMU_HFPERCLKEN0_GPIO   (0x1U << 7)
//#define 	GPIO_BASE   (0x40006000U)
//#define 	CMU_BASE   (0x400C8000U)
//#define MODE_PUSH_PULL 0b0100U
//#define GPIO_PC_CTRL_OFFSET 0x048U
//#define GPIO_PC_DOUT_SET_OFFSET 0x058U
//#define GPIO_PC_MODEH 0x050U
//#define CPU_CLOCK 24000000U
//C01 IS SHUT ON
uint32_t stack_blinky1[40] __attribute__ ((aligned (8)));
uint32_t *sp_blinky1 = &stack_blinky1[40];
uint8_t data[4];
int x = 0;
int z = 1;
int displayNumber = 264;
int DELAY;
int number;

float j;

int main()
{
  CHIP_Init();
  BSP_init();
  BSP_setLED();
  initSpi3Wire();
  updateMatrix(0.1,matrix);
  __enable_irq();
  BSP_delay(10000);
  USART1->TXDOUBLE = 0xC01;
  BSP_delay(1000);
  USART1->TXDOUBLE |= 0xB07;
  BSP_delay(1000);
  while (1) {
      
      drawMatrix(matrix);
      BSP_delay(1000);
    //BSP_delay(32768);
    //USART1->TXDOUBLE |= 0xF0;
    //writeSpiByte(0xF0,1);
  }
  
  
    /*TIMER0->CMD |= 1;
    TIMER0->IEN |= 1;
    TIMER0->TOP = (1<<14);
    TIMER0->CTRL |= (9U <<24);*/
    //TIMER0->CTRL |= 2U;

  return 0;
}


