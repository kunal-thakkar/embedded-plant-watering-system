#ifndef __STM_8_DS1302_H__
#define __STM_8_DS1302_H__

#define DS1302_SCLK_PORT   	GPIOA
#define DS1302_DATA_PORT  	GPIOA
#define DS1302_RST_PORT   	GPIOD
#define DS1302_SCLK   			GPIO_PIN_1
#define DS1302_DATA   			GPIO_PIN_2
#define DS1302_RST    			GPIO_PIN_6

extern void ds1302_init(void);
extern void set_time(unsigned char time[]);
extern void read_time(unsigned char time[]);
extern bool is_valid_time(unsigned char time[]);

#endif