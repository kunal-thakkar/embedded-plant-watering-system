#ifndef		__MAIN_H
#define		__MAIN_H

#include "stm8s.h"

#define I2C_PORT			GPIOB
#define I2C_SCL_PIN			GPIO_PIN_4
#define I2C_SDA_PIN			GPIO_PIN_5

#define OSC_PORT			GPIOA
#define OSC_IN_PIN		GPIO_PIN_1
#define OSC_OUT_PIN		GPIO_PIN_2

#define BUTTON_SET_PORT	    GPIOD
#define BUTTON_SET_PIN	    GPIO_PIN_5

#define BUTTON_UP_PORT	    GPIOD
#define BUTTON_UP_PIN		GPIO_PIN_6

#define BUTTON_DW_PORT	    GPIOD
#define BUTTON_DW_PIN		GPIO_PIN_4

#define PN2222_PORT			GPIOA
#define PN2222_PIN			GPIO_PIN_3

#define LCD_RS_PORT 		GPIOD
#define LCD_RS_PIN			GPIO_PIN_2

#define LCD_EN_PORT 		GPIOC
#define LCD_EN_PIN			GPIO_PIN_7

#define LCD_D4_PORT 		GPIOC
#define LCD_D4_PIN			GPIO_PIN_6

#define LCD_D5_PORT 		GPIOC
#define LCD_D5_PIN			GPIO_PIN_5

#define LCD_D6_PORT 		GPIOC
#define LCD_D6_PIN			GPIO_PIN_4

#define LCD_D7_PORT 		GPIOC
#define LCD_D7_PIN			GPIO_PIN_3



typedef struct 
{
	uint8_t hour;
	uint8_t minute;
	uint8_t duration;
} Schedule;

@far @interrupt void TIM2_UPD_IRQHandler(void);
@far @interrupt void TIM4_UPD_IRQHandler(void);
void _stext(void); /* RESET startup routine */
#endif