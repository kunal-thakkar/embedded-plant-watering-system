/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 */

#include "stm8s.h"
#include "stm8s_delay.h"
#include "stm8s_ds1302.h"

#define LED_PORT		GPIOA
#define LED_PIN			GPIO_PIN_3
#define PN2222_PORT GPIOD
#define PN2222_PIN 	GPIO_PIN_5
#define ON_HOUR			10
#define ON_MINUTE		0
#define ON_DURATION	30000

unsigned char time[7];

void GPIO_setup(void) { 
	/* The following lines deinitialize the GPIOs we used. 
	Every time you reconfigure or setup a hardware peripheral 
	for the first time you must deinitialize it before using it
	*/
	GPIO_DeInit(LED_PORT); 
	GPIO_DeInit(PN2222_PORT); 
	/*
	After deinitialization, we are good to go for initializing
	or setting up the GPIOs.
	*/
	GPIO_Init(LED_PORT, (GPIO_Pin_TypeDef)(LED_PIN), GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_Init(PN2222_PORT, (GPIO_Pin_TypeDef)PN2222_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
}

main() {
	GPIO_setup(); 
	ds1302_init();
	
	read_time(time);
	
	if(!is_valid_time(time)){
		//{second, minute, hour, dayOfMonth, month, dayOfWeek, year}
		unsigned char time[7] = {55, 48, 21, 21, 12, 2, 20};
		set_time(time);
	}

	while(1){
		read_time(time);
		if(!is_valid_time(time)){
			GPIO_WriteHigh(LED_PORT, LED_PIN);
		}
		else if(time[1] == ON_MINUTE && time[2] == ON_HOUR) {
			GPIO_WriteHigh(LED_PORT, (GPIO_Pin_TypeDef)LED_PIN);
			GPIO_WriteHigh(PN2222_PORT, (GPIO_Pin_TypeDef)PN2222_PIN);
			delay_ms(ON_DURATION);
			GPIO_WriteLow(LED_PORT, (GPIO_Pin_TypeDef)LED_PIN);
			GPIO_WriteLow(PN2222_PORT, (GPIO_Pin_TypeDef)PN2222_PIN);
		}
		else {
			GPIO_WriteLow(LED_PORT, LED_PIN);
		}
		delay_ms(10000);
	}
}