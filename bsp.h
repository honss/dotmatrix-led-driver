#ifndef __BSP_H__
#define __BSP_H__

#define CS_PORT   gpioPortC 
#define CS_PIN    14
#define CLK_PORT  gpioPortC 
#define CLK_PIN   15
#define DATA_PORT gpioPortD 
#define DATA_PIN  7
#define SPI_USART        USART1 
#define SPI_LOCATION     USART_ROUTE_LOCATION_LOC3 
#define SPI_USART_CLOCK  cmuClock_USART1



#include "stdint.h"
#include "em_cmu.h"
#include "intrinsics.h"
#include "em_dma.h"
#include "em_rtc.h"
#include "em_pcnt.h"
#include "em_usart.h"
#include "max7129.h"
#include "em_gpio.h"
//#include "system_efm32zg.c"
extern int counter;
extern int number;
//#include "em_system.c"
#include "em_chip.h"

/* system clock tick [Hz] */
void BSP_init(void);
void BSP_setLED(void);
void BSP_clearLED(void);

void BSP_setLED2(void);
void BSP_clearLED2(void);

/* get the current value of the clock tick counter (returns immedately) */
uint32_t BSP_tickCtr(void);

/* delay for a specified number of system clock ticks (polling) */
void BSP_delay(uint32_t ticks);

void BSP_display(int number);
void BSP_setSegment(int segment);
void BSP_clearSegment(int segment);
void BSP_write(int number);
void BSP_writeDigit(int digit);
void initSpi3Wire(void);
void writeSpiByte(uint8_t addr,uint8_t data);
#endif // __BSP_H__