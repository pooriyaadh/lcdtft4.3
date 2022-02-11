
/**
* AUTHOR :  POORIYA_ADH   TFT_LCD PROJECT (2022)
 */

#ifndef __TEMPERETURE_H__
#define __TEMPERETURE_H__

#include    "touch.h" 
#include    "lcd.h"
#include    "delay.h"
#include    "stdlib.h"
#include    "math.h"
#include    "24cxx.h"
#include    "gui.h"
#include    "test.h"
#include    "string.h"
#include    "ctype.h"
#include    "pic.h"
#include     "pic1.h"
#include     "pic2.h"
#include      "pic3.h"
#include      "pic4.h"
#include      "stop.h"
#include      "tempw.h"
#include      "MAX31865.h"
#include    "stdio.h"
#include    "picbackup.h"
#include "bachuppp3.h"
#include  "enterbutton.h"
void TestTemperature(void);

//void ds18b20initiallize(void);
#define  MAX31865_READ                   0x00
#define  MAX31865_WRITE                  0x80
#define  MAX31856_RTDMSB_REG             0x01
#define  MAX31856_CONFIG_REG             0x00
#define  MAX31856_CONFIG_BIAS            0x80
#define  MAX31856_CONFIG_1SHOT           0x20
#define  MAX31856_CONFIG_3WIRE           0x10
#define  MAX31856_CONFIG_MODEAUTO        0x40
#define     RREF 430                        // Reference resistor
#define     FACTOR 32768                    // 2^15 used for data to resistance conversion
#define     ALPHA 0.003851                  // PT-100 temperature coefficient

#define     ON       1
#define     OFF      0
#define   DELAY(x)  delay_ms(x)
#define   DS18B20_PORT    GPIOA
#define   DS18B20_PIN     GPIO_Pin_1
#define    pll_m     4
#define    pll_n     180
#define    pll_p     0
#define    TEMP_HIGH 200
#define    TEMP_LOW 20
//temperature pid params
#define    TEMP_DIFF 30
#define    RELAY_TIME_OFF 1000
#define    RELAY_TIME_ON  1000
//PID varaiables
#define    PID_OFFSET     30
#define    PID_DUTY_CYCLE 1.0


#define    NUMBERPASS1  10
#define    NUMBERPASS2  10
#define    NUMBERPASS3  10

void    menu(void);
char    str1[32],str2[32],str3[32];
void    configpioG      (void);
void    configGPIOB     (void);
uint8_t   Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t  SUM, RH, TEMP;
int   Temperature;
int    Humidity;
uint8_t  Presence = 0;
int  temph, templ, temp1,check;
int   temp;
uint32_t  dist,data;
int  i;
int tempp;
void tempraturesensor(void);
#endif







