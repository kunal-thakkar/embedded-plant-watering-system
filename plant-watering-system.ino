/**
 * Added program to run via DS1307 with Digispark AtTiny 85
 */

#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI - comment this out to use with standard arduinos
#include <DigiUSB.h>          
#include <TinyRTClib.h>

#define LED_OUT     1
#define ON_HOUR     12
#define ON_MINUTE   0
#define ON_TIME     30000

RTC_DS1307 RTC;

void setup(){  
  TinyWireM.begin();                    // initialize I2C lib - comment this out to use with standard arduinos
  RTC.begin();
  pinMode(LED_OUT, OUTPUT);
  if (! RTC.isrunning()) {
    // following line sets the RTC to the date & time this sketch was compiled only for the first run
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }  
  DigiUSB.begin();
}

void loop(){
  DateTime now = RTC.now();
  if(now.hour() == 0 && now.minute() == 0 && now.second() == 0){
    digitalWrite(LED_OUT, HIGH);
  }
  else if(now.hour() == ON_HOUR && now.minute() == ON_MINUTE && now.second() == 0){
    digitalWrite(LED_OUT, HIGH);
    delay(ON_TIME);
    digitalWrite(LED_OUT, LOW);
  }
  else {
    digitalWrite(LED_OUT, LOW);
  }
  DigiUSB.print(now.hour(), DEC);
  DigiUSB.print(":");
  DigiUSB.print(now.minute(), DEC);
  DigiUSB.print(':');
  DigiUSB.println(now.second(), DEC);
  DigiUSB.delay(1000);
}
