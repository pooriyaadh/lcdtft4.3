#define   DS18B20_PORT    GPIOA
#define   DS18B20_PIN     GPIO_Pin_1
typedef enum
{
 GPIO_PIN_RESET = 0,
 GPIO_PIN_SET
}GPIO_PinState;
uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM, RH, TEMP;
extern int Temperature;
extern int Humidity;
uint8_t Presence = 0;
int temph, templ, temp1,check;
extern  int  temp;
extern uint32_t  dist,data;
extern int  i;
extern int tempp;
void tempraturesensor(void);
void configpioG(void);





