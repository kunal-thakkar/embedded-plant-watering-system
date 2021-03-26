/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 */
#include "stm8s.h"
#include "main.h"
#include "lcd.h"
#include "stm8s_delay.h"
#include "ds1307.h"

Time time;
Schedule schedules[] = {{7, 0, 120}, {17, 0, 60}};
int16_t alarm = 0;
int16_t duration;
int32_t sec_ticks = 0;
int8_t schedule_idx = -1;
int8_t schedule_count = 2;
#define SEC_IN_HOUR 	3600
#define SEC_IN_MINUTE	60

char ch[0x09] = {0x20, 0x20, ':', 0x20, 0x20, ':', 0x20, 0x20, '\0'};
bool set = FALSE;
int8_t menu = 0;
int8_t data_value;

void clock_setup(void);
void I2C_setup(void);
void TIM2_setup(void);
void TIM4_setup(void);
void update_time(void);
void set_next_schedule(void);

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
	GPIO_Init(I2C_PORT, I2C_SCL_PIN, GPIO_MODE_OUT_OD_HIZ_FAST);
	GPIO_Init(I2C_PORT, I2C_SDA_PIN, GPIO_MODE_OUT_OD_HIZ_FAST);
}

void show_value(uint8_t x_pos, uint8_t y_pos, uint8_t value)
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
}

uint8_t adjust(uint8_t x_pos, uint8_t y_pos, signed char value_max, signed char value_min, signed char value)
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
	TIM2_setup();
	TIM4_setup();
	set_next_schedule();
	lcd_init();
	lcd_puts(0,0,(int8_t*)"Hello Booboo");
	DS1307_init();
	get_time();
	sec_ticks = ((((time.h * 60) + time.m) * 60) + time.s);
	enableInterrupts();
	while(1)
	{
		if(GPIO_ReadInputPin(BUTTON_SET_PORT, BUTTON_SET_PIN) == FALSE)
		{
			while(GPIO_ReadInputPin(BUTTON_SET_PORT, BUTTON_SET_PIN) == FALSE);
			menu++;
			if (menu >= 3) {
				menu = -1;
				set_time();
				sec_ticks = (((time.h * 60) + time.m) * 60) + time.s;
				set = FALSE;
				lcd_off_cursor();
				delay_ms(100);
				TIM2_Cmd(ENABLE);
			}
			else if (menu != -1)
			{
				TIM2_Cmd(DISABLE);
				delay_ms(100);
				set = TRUE;
				lcd_blink_cursor();
			}
		}		
		if(set)
		{
			setup_time();
		}
	};
}

void clock_setup(void)
{
	CLK_DeInit();
	
	CLK_HSECmd(ENABLE);
	CLK_LSICmd(DISABLE);
	CLK_HSICmd(DISABLE);
	while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
	
	CLK_ClockSwitchCmd(ENABLE);
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
	
	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSE, 
	DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
	
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);
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

void TIM2_setup(void)
{	
	TIM2_DeInit();
	TIM2_TimeBaseInit(TIM2_PRESCALER_512, 15625);
	TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
	TIM2_Cmd(ENABLE);
}

void TIM4_setup(void)
{	
	TIM4_DeInit();
	TIM4_TimeBaseInit(TIM4_PRESCALER_128, 250);	
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
}

void TIM2_UPD_IRQHandler(void) {
	TIM2_ClearFlag(TIM2_FLAG_UPDATE);	
	TIM2_ClearITPendingBit(TIM4_IT_UPDATE);
	sec_ticks++;
	if(sec_ticks > 86399) {
		sec_ticks = 0;
	}
	if(alarm == sec_ticks) {
		GPIO_WriteHigh(PN2222_PORT, PN2222_PIN);
		TIM4_Cmd(ENABLE);
	}
	update_time();
	if(!set) {
		display_time();
	}
}

void TIM4_UPD_IRQHandler(void) {
	duration--;
	if(duration == 0) {
		GPIO_WriteLow(PN2222_PORT, PN2222_PIN);
		TIM4_Cmd(DISABLE);
		set_next_schedule();
	}
	TIM4_ClearFlag(TIM4_FLAG_UPDATE);	
	TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}

void update_time(void) {
	int32_t _sec_ticks = sec_ticks;
	time.s = _sec_ticks % 60;
	_sec_ticks /= 60;
	time.m = _sec_ticks % 60;
	_sec_ticks /= 60;
	time.h = _sec_ticks;
}

void set_next_schedule(void) {
	schedule_idx = (schedule_idx < schedule_count) ? schedule_idx + 1 : 0;
	alarm = ((schedules[schedule_idx].hour * 60) + schedules[schedule_idx].minute) * 60;
	duration = schedules[schedule_idx].duration * 250;
}