//driver for max7219 efm32zg
#include "bsp.h"
char matrix[8];
float value;
int second;
int third;
int fourth;
int first;
void initSpi3Wire()
    {
      USART_InitSync_TypeDef usartConfig = USART_INITSYNC_DEFAULT;

      /* Enabling clock to USART and GPIO */
      CMU_ClockEnable(SPI_USART_CLOCK,true);
      CMU_ClockEnable(cmuClock_GPIO,true); 
      
      usartConfig.clockMode = usartClockMode3;
      usartConfig.msbf      = true; 

      /* Configure USART as SPI master */
      USART_InitSync(SPI_USART,&usartConfig);

      /* Enable internal lookback between TX and RX pin and enable Auto CS */
      SPI_USART->CTRL |= /*USART_CTRL_LOOPBK | */USART_CTRL_AUTOCS;

      /* Enable USART1 SPI pins (3 wire) on location 1. */
      SPI_USART->ROUTE = USART_ROUTE_CLKPEN | 
                         USART_ROUTE_TXPEN  | 
                         USART_ROUTE_CSPEN  |
                         SPI_LOCATION;

      /* Configre GPIOs for SPI pins */
      GPIO_PinModeSet(DATA_PORT, DATA_PIN, gpioModePushPull,0);/* Data */
      GPIO_PinModeSet(CLK_PORT,  CLK_PIN,  gpioModePushPull,1);/* Clock */
      GPIO_PinModeSet(CS_PORT,   CS_PIN,   gpioModePushPull,1);/* CS */
      
      USART_Enable(SPI_USART, usartEnable);
      USART1->FRAME |= 13;
      USART1->TXDOUBLE |= 0xC01;
      USART1->TXDOUBLE |= 0xB07;
 }

void writeSpiByte(uint8_t addr,uint8_t data)
{
  /* Write addr to TXDATA0 to be transmitted first, before TXDATA1 with data
   * value is sent */
  SPI_USART->TXDOUBLE = addr << _USART_TXDOUBLE_TXDATA1_SHIFT |
                        data << _USART_TXDOUBLE_TXDATA0_SHIFT;

  /* Wait for TX to complete */
  while(!(USART_StatusGet(SPI_USART)& USART_STATUS_TXC));
}


void drawMatrix(char * matrix) {
  for(int i=1;i<9;i++) {
    if(USART1->STATUS && (1<<5)){
      USART1->TXDOUBLE = (i)<<8 | matrix[i-1];
      BSP_delay(500);
    }
  }
}

void updateMatrix(float number, char * matrix) {
  //clearing matrix
  for(int i=1;i<9;i++) {
   matrix[i-1] = 0;
  }
  first=floorf(number);
  second = ((int)floorf(number*10))%10;
  third = ((int)floorf(number*100))%100%10;
  fourth = (((int)floorf(number*1000))%1000)%100%10;
  //fourth = ((((int)floorf(number*10000))%1000)%100)%10;
  //FIRST NUMBER
    if( first  == 1) {
      matrix[2-1] = 15;
  }
    else if( first  == 2) {
      matrix[1-1] = 13;
      matrix[2-1] = 11;
  }
    else if( first  == 3) {
      matrix[1-1] = 21;
      matrix[2-1] = 31;
  }
    else if( first  == 4) {
      matrix[1-1] = 3;
      matrix[2-1] = 14;
  }
    else if( first  == 5) {
      matrix[1-1] = 11;
      matrix[2-1] = 13;
  }
    else if( first  == 6) {
      matrix[1-1] = 15;
      matrix[2-1] = 13;
  }
    else if( first  == 7) {
      matrix[1-1] = 1;
      matrix[2-1] = 15;
  }
    else if( first  == 8) {
      matrix[1-1] = 15;
      matrix[2-1] = 15;
  }
    else if( first  == 9) {
      matrix[1-1] = 3;
      matrix[2-1] = 15;
  }
    else if( first  == 0) {
      matrix[1-1] = 9;
      matrix[2-1] = 9;
  }

  //second digit
   
    if( second  == 1) {
      matrix[4-1] = 15<<4;
  }
    else if( second  == 2) {
      matrix[3-1] = 13<<4;
      matrix[4-1] = 11<<4;
  }
    else if( second  == 3) {
      matrix[3-1] = 21<<3;
      matrix[4-1] = 31<<3;
  }
    else if( second  == 4) {
      matrix[3-1] = 3<<4;
      matrix[4-1] = 14<<4;
  }
    else if( second  == 5) {
      matrix[3-1] = 11<<4;
      matrix[4-1] = 13<<4;
  }
    else if( second  == 6) {
      matrix[3-1] = 15<<4;
      matrix[4-1] = 13<<4;
  }
    else if( second  == 7) {
      matrix[3-1] = 1<<4;
      matrix[4-1] = 15<<4;
  }
    else if( second  == 8) {
      matrix[3-1] = 15<<4;
      matrix[4-1] = 15<<4;
  }
    else if( second  == 9) {
      matrix[3-1] = 3<<4;
      matrix[4-1] = 15<<4;
  }
    else if( second  == 0) {
      matrix[3-1] = 9<<4;
      matrix[4-1] = 9<<4;
  }
  
  //third digit
  if( third  == 1) {
      matrix[6-1] = 15;
  }
    else if( third  == 2) {
      matrix[5-1] = 13;
      matrix[6-1] = 11;
  }
    else if( third  == 3) {
      matrix[5-1] = 21;
      matrix[6-1] = 31;
  }
    else if( third  == 4) {
      matrix[5-1] = 3;
      matrix[6-1] = 14;
  }
    else if( third  == 5) {
      matrix[5-1] = 11;
      matrix[6-1] = 13;
  }
    else if( third  == 6) {
      matrix[5-1] = 15;
      matrix[6-1] = 13;
  }
    else if( third  == 7) {
      matrix[5-1] = 1;
      matrix[6-1] = 15;
  }
    else if( third  == 8) {
      matrix[5-1] = 15;
      matrix[6-1] = 15;
  }
    else if( third  == 9) {
      matrix[5-1] = 3;
      matrix[6-1] = 15;
  }
    else if( third  == 0) {
      matrix[5-1] = 9;
      matrix[6-1] = 9;
  }
 
    //second digit
   
    if( fourth  == 1) {
      matrix[8-1] = 15<<4;
  }
    else if( fourth  == 2) {
      matrix[7-1] = 13<<4;
      matrix[8-1] = 11<<4;
  }
    else if( fourth  == 3) {
      matrix[7-1] = 21<<3;
      matrix[8-1] = 31<<3;
  }
    else if( fourth  == 4) {
      matrix[7-1] = 3<<4;
      matrix[8-1] = 14<<4;
  }
    else if( fourth  == 5) {
      matrix[7-1] = 11<<4;
      matrix[8-1] = 13<<4;
  }
    else if( fourth  == 6) {
      matrix[7-1] = 15<<4;
      matrix[8-1] = 13<<4;
  }
    else if( fourth  == 7) {
      matrix[7-1] = 1<<4;
      matrix[8-1] = 15<<4;
  }
    else if( fourth  == 8) {
      matrix[7-1] = 15<<4;
      matrix[8-1] = 15<<4;
  }
    else if( fourth  == 9) {
      matrix[7-1] = 3<<4;
      matrix[8-1] = 15<<4;
  }
    else if( fourth  == 0) {
      matrix[7-1] = 9<<4;
      matrix[8-1] = 9<<4;
  }
  
}