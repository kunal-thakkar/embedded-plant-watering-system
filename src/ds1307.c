#include "DS1307.h"

extern Time time;
extern void delay_ms(unsigned int  value);

void DS1307_init(void)
{
  DS1307_write(control_reg, 0x00);
}

int8_t DS1307_read(int8_t address)
{
	int8_t value = 0x00;

	I2C_GenerateSTART(ENABLE);
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(DS1307_WR, I2C_DIRECTION_TX); 
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C_SendData(address); 
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTART(ENABLE);
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(DS1307_RD, I2C_DIRECTION_RX);
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	delay_ms(10);
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED));
	
	value = I2C_ReceiveData();
	I2C_AcknowledgeConfig(I2C_ACK_NONE);
	I2C_GenerateSTOP(ENABLE);   
	return value;
}

void DS1307_write(int8_t address, int8_t value)
{
	I2C_GenerateSTART(ENABLE);
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(DS1307_WR, I2C_DIRECTION_TX); 
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	
	I2C_SendData(address); 
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(value);
	while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTOP(ENABLE);   
}

int8_t bcd_to_decimal(int8_t value)
{
    return ((value & 0x0F) + (((value & 0xF0) >> 0x04) * 0x0A));
}

int8_t decimal_to_bcd(int8_t value)
{
    return (((value / 0x0A) << 0x04) & 0xF0) | ((value % 0x0A) & 0x0F);
}

void get_time(void)
{
	time.s = bcd_to_decimal(DS1307_read(sec_reg));
	time.m = bcd_to_decimal(DS1307_read(min_reg));
	time.h = bcd_to_decimal(DS1307_read(hr_reg));
	time.dy = bcd_to_decimal(DS1307_read(day_reg));
	time.dt = bcd_to_decimal(DS1307_read(date_reg));
	time.mt = bcd_to_decimal(DS1307_read(month_reg));
	time.yr = bcd_to_decimal(DS1307_read(year_reg));
}

void set_time(void)
{
	DS1307_write(sec_reg, decimal_to_bcd(time.s));
	DS1307_write(min_reg, decimal_to_bcd(time.m));
	DS1307_write(hr_reg, decimal_to_bcd(time.h));
	DS1307_write(day_reg, decimal_to_bcd(time.dy));
	DS1307_write(date_reg, decimal_to_bcd(time.dt));
	DS1307_write(month_reg, decimal_to_bcd(time.mt));
	DS1307_write(year_reg, decimal_to_bcd(time.yr));
}

void set_freq(char data)
{
	DS1307_write(control_reg, data);
}