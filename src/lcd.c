#include "lcd.h"
#include "stm8s_delay.h"

/*--------------- Initialize LCD ------------------*/
void lcd_init(void)
{
	//Config output for text lcd
	GPIO_Init(LCD_RS_PORT,LCD_RS_PIN,GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(LCD_EN_PORT,LCD_EN_PIN,GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(LCD_D4_PORT,LCD_D4_PIN,GPIO_MODE_OUT_PP_LOW_FAST);  
	GPIO_Init(LCD_D5_PORT,LCD_D5_PIN,GPIO_MODE_OUT_PP_LOW_FAST);  
	GPIO_Init(LCD_D6_PORT,LCD_D6_PIN,GPIO_MODE_OUT_PP_LOW_FAST);  
	GPIO_Init(LCD_D7_PORT,LCD_D7_PIN,GPIO_MODE_OUT_PP_LOW_FAST);  

	delay_ms(40);
	
	PIN_LOW(LCD_D4_PORT, LCD_D4_PIN);
	PIN_HIGH(LCD_D5_PORT, LCD_D5_PIN);
	PIN_LOW(LCD_D6_PORT, LCD_D6_PIN);
	PIN_LOW(LCD_D7_PORT, LCD_D7_PIN);
	PIN_LOW(LCD_RS_PORT, LCD_RS_PIN);
	
	PIN_HIGH(LCD_EN_PORT, LCD_EN_PIN);
	PIN_LOW(LCD_EN_PORT, LCD_EN_PIN);
	
	lcd_write(0,0x28);
	lcd_write(0,0x0c);
	lcd_write(0,0x06);
	lcd_write(0,0x01);
}

/*--------------- Write To LCD ---------------*/
void lcd_write(uint8_t type,uint8_t data)
{
    delay_ms(2);
    if(type)
    {
        PIN_HIGH(LCD_RS_PORT,LCD_RS_PIN);
    }else
    {
        PIN_LOW(LCD_RS_PORT,LCD_RS_PIN);
    }
    
    //Send High Nibble
    if(data&0x80)
    {
        PIN_HIGH(LCD_D7_PORT,LCD_D7_PIN);
    }else
    {
        PIN_LOW(LCD_D7_PORT,LCD_D7_PIN);
    }
    
    if(data&0x40)
    {
        PIN_HIGH(LCD_D6_PORT,LCD_D6_PIN);
    }else
    {
        PIN_LOW(LCD_D6_PORT,LCD_D6_PIN);
    }
    
    if(data&0x20)
    {
        PIN_HIGH(LCD_D5_PORT,LCD_D5_PIN);
    }else
    {
        PIN_LOW(LCD_D5_PORT,LCD_D5_PIN);
    }
    
    if(data&0x10)
    {
        PIN_HIGH(LCD_D4_PORT,LCD_D4_PIN);
    }else
    {
        PIN_LOW(LCD_D4_PORT,LCD_D4_PIN);
    }
    PIN_HIGH(LCD_EN_PORT,LCD_EN_PIN);
    PIN_LOW(LCD_EN_PORT,LCD_EN_PIN);

    //Send Low Nibble
    if(data&0x08)
    {
        PIN_HIGH(LCD_D7_PORT,LCD_D7_PIN);
    }else
    {
        PIN_LOW(LCD_D7_PORT,LCD_D7_PIN);
    }
    
    if(data&0x04)
    {
        PIN_HIGH(LCD_D6_PORT,LCD_D6_PIN);
    }else
    {
        PIN_LOW(LCD_D6_PORT,LCD_D6_PIN);
    }
    
    if(data&0x02)
    {
        PIN_HIGH(LCD_D5_PORT,LCD_D5_PIN);
    }else
    {
        PIN_LOW(LCD_D5_PORT,LCD_D5_PIN);
    }
    
    if(data&0x01)
    {
        PIN_HIGH(LCD_D4_PORT,LCD_D4_PIN);
    }else
    {
        PIN_LOW(LCD_D4_PORT,LCD_D4_PIN);
    }
    PIN_HIGH(LCD_EN_PORT,LCD_EN_PIN);
    PIN_LOW(LCD_EN_PORT,LCD_EN_PIN);
}

void lcd_puts(uint8_t x, uint8_t y, int8_t *string)
{
    //Set Cursor Position
    #ifdef LCD16xN	//For LCD16x2 or LCD16x4
    switch(y)
    {
      case 0: //Row 0
          lcd_write(0,0x80+0x00+x);
          break;
      case 1: //Row 1
          lcd_write(0,0x80+0x40+x);
          break;
      case 2: //Row 2
          lcd_write(0,0x80+0x10+x);
          break;
      case 3: //Row 3
          lcd_write(0,0x80+0x50+x);
          break;
    }
    #endif
    #ifdef LCD20xN	//For LCD20x4
    switch(y)
    {
      case 0: //Row 0
          lcd_write(0,0x80+0x00+x);
          break;
      case 1: //Row 1
          lcd_write(0,0x80+0x40+x);
          break;
      case 2: //Row 2
          lcd_write(0,0x80+0x14+x);
          break;
      case 3: //Row 3
          lcd_write(0,0x80+0x54+x);
          break;
    }
    #endif
    while(*string)
    {
        lcd_write(1,*string);
        string++;
    }
}

void lcd_clear(void)
{
    lcd_write(0,0x01);
}

void lcd_blink_cursor(void)
{
	lcd_write(0, 0x0E);
}

void lcd_off_cursor(void)
{
	lcd_write(0, 0x0C);
}
