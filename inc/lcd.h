#ifndef		__LCDTXT_H
#define		__LCDTXT_H

#include "stm8s.h"
#include "main.h"

/*------------- Define LCD Use -----------------*/
/*Note: Comment which not use */

#define LCD16xN //For lcd16x2 or lcd16x4
//#define LCD20xN //For lcd20x4

/*------------ Declaring Private Macro -----------------*/

#define PIN_LOW(PORT,PIN) GPIO_WriteLow(PORT,PIN);
#define PIN_HIGH(PORT,PIN) GPIO_WriteHigh(PORT,PIN);
/*------------ Declaring Function Prototype -------------*/
void lcd_init(void);
void lcd_write(uint8_t type,uint8_t data);
void lcd_puts(uint8_t x, uint8_t y, int8_t *string);
void lcd_clear(void);
void lcd_blink_cursor(void);
void lcd_off_cursor(void);
#endif