/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 */

#include "stm8s.h"
#include "lcd.h"
#include "stm8s_delay.h"
#include "ds1307.h"

#define BUTTON_SET_PORT	GPIOD
#define BUTTON_SET_PIN	GPIO_PIN_3

#define BUTTON_UP_PORT	GPIOD
#define BUTTON_UP_PIN		GPIO_PIN_5

#define BUTTON_DW_PORT	GPIOD
#define BUTTON_DW_PIN		GPIO_PIN_6

#define PN2222_PORT					GPIOD
#define PN2222_PIN					GPIO_PIN_4

struct
{
 unsigned char s;
 unsigned char m;
 unsigned char h;
 unsigned char dy;
 unsigned char dt;
 unsigned char mt;
 unsigned char yr;
}time;

struct
{
	unsigned char hour;
	unsigned char minute;
	unsigned char duration;
} schedule = { 0x0A, 0x00, 0x3B };

char ch[0x09] = {0x20, 0x20, ':', 0x20, 0x20, ':', 0x20, 0x20, '\0'};

bool set = FALSE;
bool motor_on = FALSE;

unsigned char menu = 0;
unsigned char data_value;

void clock_setup(void);
void I2C_setup(void);

void GPIO_setup(void) { 
	/* The following lines deinitialize the GPIOs we used. 
	Every time you reconfigure or setup a hardware peripheral 
	for the first time you must deinitialize it before using it
	*/
	GPIO_DeInit(GPIOA); 
	GPIO_DeInit(GPIOB); 
	GPIO_DeInit(GPIOC); 
	GPIO_DeInit(GPIOD); 
	/*
	After deinitialization, we are good to go for initializing
	or setting up the GPIOs.
	*/
	GPIO_Init(PN2222_PORT, PN2222_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(BUTTON_SET_PORT, BUTTON_SET_PIN, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(BUTTON_UP_PORT, BUTTON_UP_PIN, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(BUTTON_DW_PORT, BUTTON_DW_PIN, GPIO_MODE_IN_PU_NO_IT);
	// Pin for I2C communications
	GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_FAST);
	GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_FAST);
}

void show_value(unsigned char x_pos, unsigned char y_pos, unsigned char value)
{
	char ch[0x03] = {0x20, 0x20, '\0'};
	ch[0] = (((value / 10) % 10) + 0x30);
	ch[1] = ((value % 10) + 0x30);
	lcd_puts(x_pos, y_pos, (int8_t*)ch);
}

void display_time(void)
{	
	ch[0] = (((time.h / 10) % 10) + 0x30);
	ch[1] = ((time.h % 10) + 0x30);
	ch[3] = (((time.m / 10) % 10) + 0x30);
	ch[4] = ((time.m % 10) + 0x30);
	ch[6] = (((time.s / 10) % 10) + 0x30);
	ch[7] = ((time.s % 10) + 0x30);
	lcd_puts(4, 1, (int8_t*)ch);
	delay_ms(100);
}

unsigned char adjust(unsigned char x_pos, unsigned char y_pos, signed char value_max, signed char value_min, signed char value)
{
	if(GPIO_ReadInputPin(BUTTON_UP_PORT, BUTTON_UP_PIN) == FALSE)
	{
		while(GPIO_ReadInputPin(BUTTON_UP_PORT, BUTTON_UP_PIN) == FALSE);
		value++;
		if (value > value_max) value = value_min;
	}
	if(GPIO_ReadInputPin(BUTTON_DW_PORT, BUTTON_DW_PIN) == FALSE)
	{
		while(GPIO_ReadInputPin(BUTTON_DW_PORT, BUTTON_DW_PIN) == FALSE);
		value--;
		if (value < value_min) value = value_max;
	}	
	show_value(x_pos, y_pos, value);
	return value;
}

void setup_time(void)
{
	switch(menu)
	{
		case 0:
		{
			time.h = adjust(4, 1, 23, 0, time.h);
			break;
		}		
		case 1:
		{
			time.m = adjust(7, 1, 59, 0, time.m);
			break;
		}
		case 2:
		{
			time.s = adjust(10, 1, 59, 0, time.s);
			break;
		}
	}
}

main() {
	clock_setup();
	GPIO_setup(); 
	I2C_setup();
	DS1307_init();
	set_freq(0x10);
	lcd_init();
	lcd_puts(0,0,(int8_t*)"Hello Booboo");

	while(1)
	{
		if(GPIO_ReadInputPin(BUTTON_SET_PORT, BUTTON_SET_PIN) == FALSE)
		{
			while(GPIO_ReadInputPin(BUTTON_SET_PORT, BUTTON_SET_PIN) == FALSE);
			menu++;
			if (menu >= 3) {
				menu = -1;
				set_time();
				set = FALSE;
				lcd_off_cursor();
			}
			else if (menu != -1)
			{
				set = TRUE;
				lcd_blink_cursor();
			}
		}		
		if(set)
		{
			setup_time();
		}
		else
		{
			get_time();
			display_time();
		}
		if(motor_on == FALSE && time.h == schedule.hour && time.m == schedule.minute && time.s == 0)
		{
			PIN_HIGH(PN2222_PORT, PN2222_PIN);
			motor_on = TRUE;
		}
		else if (motor_on == TRUE && time.s >= schedule.duration)
		{
			PIN_LOW(PN2222_PORT, PN2222_PIN);
			motor_on = FALSE;
		}
	};
}

void clock_setup(void)
{
	CLK_DeInit();
	
	CLK_HSECmd(DISABLE);
	CLK_LSICmd(DISABLE);
	CLK_HSICmd(ENABLE);
	while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
	
	CLK_ClockSwitchCmd(ENABLE);
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV8);
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV2);
	
	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, 
	DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
	
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
}

void I2C_setup(void)
{
	I2C_DeInit();
	I2C_Init(100000, 
				  DS1307_addr, 
				  I2C_DUTYCYCLE_2, 
				  I2C_ACK_CURR, 
				  I2C_ADDMODE_7BIT, 
				  (CLK_GetClockFreq() / 1000000));
	I2C_Cmd(ENABLE);
}