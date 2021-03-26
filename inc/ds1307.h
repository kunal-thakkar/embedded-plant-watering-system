#include "stm8s_i2c.h"

#define sec_reg                     0x00
#define min_reg                     0x01
#define hr_reg                      0x02
#define day_reg                     0x03
#define date_reg                    0x04
#define month_reg                   0x05
#define year_reg                    0x06
#define control_reg                 0x07

#define DS1307_addr                 0xD0
#define DS1307_WR                   DS1307_addr
#define DS1307_RD                 	0xD1

#define FREQ_1_Hz										0x10;
#define FREQ_4_096_KHz							0x11;
#define FREQ_8_192_KHz							0x12;
#define FREQ_32_768_KHz							0x13;

typedef struct
{
  int32_t s;
  int32_t m;
  int32_t h;
  int32_t dy;
  int32_t dt;
  int32_t mt;
  int32_t yr;
} Time;

void DS1307_init(void);
int8_t DS1307_read(int8_t address);
void DS1307_write(int8_t address, int8_t value);
int8_t bcd_to_decimal(int8_t value);
int8_t decimal_to_bcd(int8_t value);
void get_time(void);
void set_time(void);
void set_freq(char data);