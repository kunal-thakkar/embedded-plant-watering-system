// Software I2C connections
sbit Soft_I2C_Scl           at RC3_bit;
sbit Soft_I2C_Sda           at RC4_bit;
sbit Soft_I2C_Scl_Direction at TRISC3_bit;
sbit Soft_I2C_Sda_Direction at TRISC4_bit;
// End Software I2C connections

/***************************************************************************************************
  Commonly used Ds1307 macros/Constants
  ***************************************************************************************************
Below values are fixed and should not be changed. Refer Ds1307 DataSheet for more info*/

#define C_Ds1307ReadMode_U8   0xD1u  // DS1307 ID
#define C_Ds1307WriteMode_U8  0xD0u  // DS1307 ID

#define C_Ds1307SecondRegAddress_U8   0x00u   // Address to access Ds1307 SEC register
#define C_Ds1307DateRegAddress_U8     0x04u   // Address to access Ds1307 DATE register
#define C_Ds1307ControlRegAddress_U8  0x07u   // Address to access Ds1307 CONTROL register
/**************************************************************************************************/

#define SET_BUTTON  PORTA.F0
#define UP_BUTTON  PORTA.F1
#define DW_BUTTON  PORTA.F2


// LCD module connections
sbit LCD_RS at RB2_bit;
sbit LCD_EN at RB3_bit;
sbit LCD_D4 at RB4_bit;
sbit LCD_D5 at RB5_bit;
sbit LCD_D6 at RB6_bit;
sbit LCD_D7 at RB7_bit;

sbit LCD_RS_Direction at TRISB2_bit;
sbit LCD_EN_Direction at TRISB3_bit;
sbit LCD_D4_Direction at TRISB4_bit;
sbit LCD_D5_Direction at TRISB5_bit;
sbit LCD_D6_Direction at TRISB6_bit;
sbit LCD_D7_Direction at TRISB7_bit;
// End LCD module connections

unsigned short read_ds1307(unsigned short address) {
  unsigned short r_data;
  Soft_I2C_Start();
  Soft_I2C_write(0xD0); //address 0x68 followed by direction bit (0 for write, 1 for read) 0x68 followed by 0 --> 0xD0
  Soft_I2C_write(address);
  Soft_I2C_Start();
  Soft_I2C_write(0xD1); //0x68 followed by 1 --> 0xD1
  r_data = Soft_I2C_Read(0);
  Soft_I2C_Stop();
  return(r_data);
}

void write_ds1307(unsigned short address,unsigned short w_data) {
  Soft_I2C_Start(); // issue I2C start signal
  //address 0x68 followed by direction bit (0 for write, 1 for read) 0x68 followed by 0 --> 0xD0
  Soft_I2C_Write(0xD0); // send byte via I2C (device address + W)
  Soft_I2C_Write(address); // send byte (address of DS1307 location)
  Soft_I2C_Write(w_data); // send data (data to be written)
  Soft_I2C_Stop(); // issue I2C stop signal
}

unsigned char BCD2UpperCh(unsigned char bcd) {
  return ((bcd >> 4) + '0');
}

unsigned char BCD2LowerCh(unsigned char bcd) {
  return ((bcd & 0x0F) + '0');
}

int Binary2BCD(int a)
{
  int t1, t2;
  t1 = a%10;
  t1 = t1 & 0x0F;
  a = a/10;
  t2 = a%10;
  t2 = 0x0F & t2;
  t2 = t2 << 4;
  t2 = 0xF0 & t2;
  t1 = t1 | t2;
  return t1;
}

int BCD2Binary(int a)
{
  int r,t;
  t = a & 0x0F;
  r = t;
  a = 0xF0 & a;
  t = a >> 4;
  t = 0x0F & t;
  r = t*10 + r;
  return r;
}

int second;
int minute;
int hour;
int hr;
int day;
int dday;
int month;
int year;
int ap;

unsigned short set_count = 0, cursor_cmd = 0x00;
short set;

char datetime[] = "00 MMM 00:00:00";

//------------------ Performs project-wide init
void Init_Main() {
  ADCON1 = 0x06;  // To turn off analog to digital converters
  // Make PORT B as output
  TRISB = 0;
  PORTB = 0xFF;
  TRISB = 0xff;
  
  //Make PORT A for INPUT
  TRISA = 0x07;
  PORTA = 0x00;
  
  Soft_I2C_Init();                              // Initialize Soft I2C communication
  Soft_I2C_Start();                              // Start I2C communication
  Soft_I2C_Write(C_Ds1307WriteMode_U8);         // Connect to DS1307 by sending its ID on I2c Bus
  Soft_I2C_Write(C_Ds1307ControlRegAddress_U8); // Select the Ds1307 ControlRegister to configure Ds1307
  Soft_I2C_Write(0x00);                         // Write 0x00 to Control register to disable SQW-Out
  Soft_I2C_Stop();                              // Stop I2C communication after initializing DS1307
  
  Lcd_Init();                // Initialize LCD
  Lcd_Cmd(_LCD_CLEAR);       // Clear LCD display
  Lcd_Cmd(_LCD_CURSOR_OFF);  // Turn cursor off
}

void readRTC() {
    second = read_ds1307(0);
    minute = read_ds1307(1);
    hour = read_ds1307(2);
    // hr = hour & 0b00011111;
    // ap = hour & 0b00100000;
    dday = read_ds1307(3);
    day = read_ds1307(4);
    month = read_ds1307(5);
    year = read_ds1307(6);
}

void setDisplay() {
  char *mmm;
  switch(month) {
    case 1:{ mmm = "Jan"; break; }
    case 2:{ mmm = "Feb"; break; }
    case 3:{ mmm = "Mar"; break; }
    case 4:{ mmm = "Apr"; break; }
    case 5:{ mmm = "May"; break; }
    case 6:{ mmm = "Jun"; break; }
    case 7:{ mmm = "Jul"; break; }
    case 8:{ mmm = "Aug"; break; }
    case 9:{ mmm = "Sep"; break; }
    case 10:{ mmm = "Oct"; break; }
    case 11:{ mmm = "Nov"; break; }
    case 12:{ mmm = "Dec"; break; }
  }    
  datetime[0] = BCD2UpperCh(day);
  datetime[1] = BCD2LowerCh(day);
  datetime[3] = mmm[0];
  datetime[4] = mmm[1];
  datetime[5] = mmm[2];
  datetime[7] = BCD2UpperCh(hour);
  datetime[8] = BCD2LowerCh(hour);
  datetime[10] = BCD2UpperCh(minute);
  datetime[11] = BCD2LowerCh(minute);
  datetime[13] = BCD2UpperCh(second);
  datetime[14] = BCD2LowerCh(second);    
  Lcd_out(1, 1, datetime);
}

//----------------- Main procedure
void main() {
  Init_Main();               // Perform initialization
  while (1) {                // Endless loop
    set = 0;
    if(SET_BUTTON == 0) {
      while(SET_BUTTON == 0);
      set_count++;
      switch (set_count)
      {
      case 1:
        cursor_cmd = 0x88;  //bring cursor to hour position
        break;
      case 2:
        cursor_cmd = 0x8B;  //bring cursor to minute position
        break;
      case 3:
        cursor_cmd = 0x8E;  //bring cursor to seconds position
        break;
      case 4:
        cursor_cmd = 0x81;  //bring cursor to day position
        break;
      case 5:
        cursor_cmd = 0x85;  //bring cursor to month position
        break;
      default:
        cursor_cmd = 0x00;
        set_count = 0;      
        break;
      }
      if(set_count == 0){
        Lcd_Cmd(_LCD_CURSOR_OFF);  // Turn cursor off
      }
      else {
        Lcd_Cmd(0x0E);  // Turn cursor blinking
      }
    }
    if(set_count) {
      if(UP_BUTTON == 0) {
        while(UP_BUTTON == 0);
        set = 1;
      }      
      if(DW_BUTTON == 0) {
        while(DW_BUTTON == 0);
        set = -1;
      }
      if(set_count && set) {
        switch(set_count)
        {
          case 1:
		      hour = BCD2Binary(hour);
          hour = hour + set;
          hour = Binary2BCD(hour);
          if((hour & 0x1F) >= 0x13) {
            hour = hour & 0b11100001;
            hour = hour ^ 0x20;
          }
          else if((hour & 0x1F) <= 0x00) {
            hour = hour | 0b00010010;
            hour = hour ^ 0x20;
          }
          write_ds1307(2, hour); //write hour
          break;
          case 2:
          minute = BCD2Binary(minute);
          minute = minute + set;
          if(minute >= 60)
          minute = 0;
          if(minute < 0)
          minute = 59;
          minute = Binary2BCD(minute);
          write_ds1307(1, minute); //write min
          break;
          case 3:
          write_ds1307(0,0x00); //Reset second to 0 sec. and start Oscillator
          break;
          case 4:
          day = BCD2Binary(day);
          day = day + set;
          day = Binary2BCD(day);
          if(day >= 0x32)
          day = 1;
          if(day <= 0)
          day = 0x31;
          write_ds1307(4, day); // write date 17
          break;
          case 5:
          month = BCD2Binary(month);
          month = month + set;
          month = Binary2BCD(month);
          if(month > 0x12)
          month = 1;
          if(month <= 0)
          month = 0x12;
          write_ds1307(5,month); // write month 6 June
          break;
          case 6:
          year = BCD2Binary(year);
          year = year + set;
          year = Binary2BCD(year);
          if(year <= -1)
          year = 0x99;
          if(year >= 0x50)
          year = 0;
          write_ds1307(6, year); // write year
          break;
        }
      }
    }
    readRTC();
    setDisplay();
    if(cursor_cmd){
      Lcd_Cmd(cursor_cmd);
    }
    Delay_ms(100);
  }
}