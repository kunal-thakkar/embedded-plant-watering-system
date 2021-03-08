#ifndef DEFINES_H
#define DEFINES_H

#include "main.h"
#define STM32F0xx
//RS - Register select pin
#define HD44780_RS_PORT		LCD_RS_GPIO_Port
#define HD44780_RS_PIN      LCD_RS_Pin
//E - Enable pin
#define HD44780_E_PORT      LCD_EN_GPIO_Port
#define HD44780_E_PIN       LCD_EN_Pin
//D4 - Data 4 pin
#define HD44780_D4_PORT     LCD_D4_GPIO_Port
#define HD44780_D4_PIN      LCD_D4_Pin
//D5 - Data 5 pin
#define HD44780_D5_PORT     LCD_D5_GPIO_Port
#define HD44780_D5_PIN      LCD_D5_Pin
//D6 - Data 6 pin
#define HD44780_D6_PORT     LCD_D6_GPIO_Port
#define HD44780_D6_PIN      LCD_D6_Pin
//D7 - Data 7 pin
#define HD44780_D7_PORT     LCD_D7_GPIO_Port
#define HD44780_D7_PIN      LCD_D7_Pin

#endif
