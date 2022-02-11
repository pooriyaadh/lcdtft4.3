void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));
  assert_param(IS_GPIO_PIN_ACTION(PinState));

  if(PinState != GPIO_PIN_RESET)
  {
    GPIOx->BSRRH = GPIO_Pin;
  }
  else
  {
    GPIOx->BSRRL = (uint32_t)GPIO_Pin << 16U;
  }
}
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  GPIO_PinState bitstatus;
  /* Check the parameters */
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  if((GPIOx->IDR & GPIO_Pin) != (uint32_t)GPIO_PIN_RESET)
  {
    bitstatus = GPIO_PIN_SET;
  }
  else
  {
    bitstatus = GPIO_PIN_RESET;
  }
  return bitstatus;
}
void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = 	GPIO_Low_Speed;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}
uint8_t DS18B20_Start(void)
{
	signed int Response = 0;
	Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);   // set the pin as output
	HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);  // pull the pin low
	delay_us (480);   // delay according to datasheet

	Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);    // set the pin as input
	delay_us (80);    // delay according to datasheet

	if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN))) Response = 1;    // if the pin is low i.e the presence pulse is detected
	else Response = -1;

	delay_us (400); // 480 us delay totally.

	return Response;
}
void DS18B20_Write (uint8_t data)
{
	Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);  // set as output
	for (i=0; i<8; i++)
	{
		if ((data & (1<<i))!=0)  // if the bit is high
		{
			// write 1
			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);  // set as output
			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);  // pull the pin LOW
			delay_us (1);  // wait for 1 us
			Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);  // set as input
			delay_us (50);  // wait for 60 us
		}
		else  // if the bit is low
		{
			// write 0
			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);
			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);  // pull the pin LOW
			delay_us (50);  // wait for 60 us
			Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);
		}
	}
}
uint8_t DS18B20_Read (void)
{
	uint8_t value=0;
	Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);
	for (i=0;i<8;i++)
	{
		Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);   // set as output

		HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);  // pull the data pin LOW
		delay_us (1);  // wait for > 1us

		Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);  // set as input
		if(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN))  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		delay_us (50);  // wait for 60 us
	}
	return value;

}
void tempraturesensor(void)
{
	configpioG();
	while(1)
	{
		Presence = DS18B20_Start ();
	   delay_ms(1000);
	   DS18B20_Write (0xCC);  // skip ROM
	   DS18B20_Write (0x44);  // convert t
	   delay_ms(1000);

	   Presence = DS18B20_Start();
     delay_ms(1000);
     DS18B20_Write (0xCC);  // skip ROM
     DS18B20_Write (0xBE);  // Read Scratch-pad

     Temp_byte1 = DS18B20_Read();
	   Temp_byte2 = DS18B20_Read();
	   TEMP = (Temp_byte2<<8)|Temp_byte1;
	   Temperature  = TEMP/16;
		
	   delay_ms(1000);
		 LCD_ShowString(40,400,16,"TEMP:" ,1);
		 LCD_ShowNum(40+100,400,Temperature,3,16);
		 LCD_ShowString(40+150,400,16, "^C" ,1);
		 if(Temperature  > 20)
		 {
			GPIOG->BSRRL |= (1 << 14);
		 }
	}	
}
void configpioG(void)
{
RCC->AHB1ENR |= (1<<6);
//GPIOA->MODER |= (1 << 10);
GPIOG->MODER = 0x55555555;
GPIOG->OTYPER = 0;
GPIOG->OSPEEDR = 0;	
}
