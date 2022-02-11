
#include 	<temperatureTest.h>
typedef enum
{
 GPIO_PIN_RESET = 0,
 GPIO_PIN_SET
}GPIO_PinState;
typedef enum
{
	STATE_HOME,
	STATE_SETTING
}GUI_STATE;

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
	Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);	 // set the pin as output
	HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);	// pull the pin low
	delay_us (480); //delay according to datasheet

	Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);		// set the pin as input
	delay_us (80);		// delay according to datasheet

	 if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN))) Response = 1;		// if the pin is low i.e the presence pulse is detected
	 else Response = -1;

	 delay_us (400); // 480 us delay totally.

	 return Response;
}
void DS18B20_Write (uint8_t data)
{
	Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);	// set as output
	for (i=0; i<8; i++)
	{
		if ((data & (1<<i))!=0)	// if the bit is high
		{
			// write 1
			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);	// set as output
			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);	// pull the pin LOW
			delay_us (1);	// wait for 1 us
			Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);	// set as input
			delay_us (50);	// wait for 60 us
		}
		else	// if the bit is low
		{
			// write 0
			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);
			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);	// pull the pin LOW
			delay_us (50);	// wait for 60 us
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
		Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);	 // set as output
		
		HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);	// pull the data pin LOW
		delay_us (1);	// wait for > 1us

		Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);	// set as input
		if(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN))	// if the pin is HIGH
		{
			value |= 1<<i;	// read = 1
		}
		delay_us (50);	// wait for 60 us
	}
	return value;
}
void TestTemperature(void)
{
			u8	 key;	
			u16	tt = 100;
			u16	tempSetPoint = 100;
			int runStop = 0;
			int forwardReverse = 0;  //0 means nothing 1 is forward 2 is reverse motion
			u16	pp = 10;
			volatile int	i;
			volatile int	j; 
			volatile GUI_STATE state;
			int refresh_all = 1;
			int refresh_temp_show = 1;
			int refresh_temp_set = 0;
			u16	pos_temp[2]; //used for storing x y position
			state = STATE_HOME;		
			tp_dev.sta	= 0;	//Eliminate the trigger signal
			tp_dev.xfac = 0;	//xfacIt is used to mark whether it has been calibrated, so it must be cleared before calibration! To avoid errors 
			configpioG(); //config the GPIOG clock
			configGPIOB(); // config the GPIOB clock	
			GPIOG->BSRRL = (1 << 6); // set the pin low 
			GPIOG->BSRRL |= (1 << 8);	// set the pin as low 
			TP_Init();
			//KEY_Init();
			//LED_Init();
		 while(1)
		 {
		 //key = KEY_Scan(2);
			tp_dev.scan(0); //Scan physical coordinates	 *STAR FOR BUG		 
			/*if(tp_dev.sta & TP_PRES_DOWN)	//IF THE TOUCH SCREEN IS PRESS
			{
			//TP_Draw_Big_Point(tp_dev.x,tp_dev.y,POINT_COLOR);
			//LCD_ShowString(40,300,16," x :",1);
			//LCD_ShowNum(40+100,300,tp_dev.x,3,16);
			//LCD_ShowString(40,400,16," y :",1);
			//LCD_ShowNum(40+100,400,tp_dev.y,3,16);
			*///}				
		
			//if(tp_dev.sta &=~ (1<<6));//The mark button has been processed.
			 

			if(state==STATE_HOME)
			{
				//draw
				if(refresh_all==1)
				{		
				LCD_direction(1); 
				LCD_Clear(LIGHTBLUE);
				Gui_Drawbmp18	(120	, 70 ,	gImage_FORWARDTRUE); //DRAW THE FORWARD PICTURE
				Gui_Drawbmp18	(20 , 10 ,		gImage_home33s); //DRAW THE PICTURE
				Gui_Drawbmp18	(20	, 220 ,	gImage_qq_logo); //DRAW THE PICTURE
				Gui_Drawbmp18	(250 , 80,		gImage_run33s); //DRAW THE PICTURE
				Gui_Drawbmp18	(120 , 170 ,	gImage_reverse32s); //DRAW THE PICTURE
				Gui_Drawbmp18	(250	, 170,	gImage_stop33s); //DRAW THE PICTURE		
				Gui_Drawbmp18	(380	, 220,	gImage_temp33s); //DRAW THE PICTURE	 
				Gui_Drawbmp18	(388	, 50 ,	gImage_FORWARDTRUE); //temp up
				Gui_Drawbmp18	(388	, 133 ,	gImage_reverse32s); //temp down
				LCD_ShowString (250,50,16," SET:", 1);
					
					if(runStop==1)
					LCD_ShowString(30,200,16,"running" ,1);
					if(runStop==0)
					LCD_ShowString(30,200,16,"stopped" ,1);
					if(forwardReverse==1)
					LCD_ShowString(30,100,16,"forward" ,1);
					if(forwardReverse==2)
					LCD_ShowString(30,100,16,"reverse" ,1);
					
					
					
					
				refresh_all = 0;	
				delay_ms(50);
				}
				if(refresh_temp_show == 1) //show the setpoint for temprature
				{		
					LCD_ShowNum(250+50,50,tt,4,16); //show the number of setpoint temperature
					refresh_temp_show = 0;
					
				}				
		//touch
		if(tp_dev.sta & TP_PRES_DOWN)	//IF THE TOUCH SCREEN IS PRESSED DOWN
		{
			pos_temp[0] = tp_dev.x;
			pos_temp[1] = tp_dev.y;
			
			//LCD_DrawRectangle(pos_temp[0] - 50,pos_temp[1] - 50,pos_temp[0] + 50,pos_temp[1] + 50);
			//LCD_ShowNum(300,200,tp_dev.x,3,16);
			//LCD_ShowNum(300,300,tp_dev.y,3,16);
			//TP_Draw_Big_Point(tp_dev.y,272 - tp_dev.x,POINT_COLOR);
			
		}
		if((tp_dev.sta&0xc0) == TP_CATH_PRES)//The button was pressed once (the button is now released.)
		 {
			 //state_switch=1;
			 
			 pos_temp[0] = tp_dev.x;
			 pos_temp[1] = tp_dev.y;
			if(abs(pos_temp[0]-244)<50&&abs(pos_temp[1]-418)<50)	//if the temperature up is pressed
			 {				
				tt++; //THE NUMBER OF SETPOINT INCREASE
				if(tt > 200) // IF THE NUMBER OF SETPOINT EQUAL TO 200 THEN THE SETPOINT GO TO THE 100
				 {
					tt = 100;
				 }
				 delay_ms(200);
				 refresh_temp_show = 1;
			 }
			 if(abs(pos_temp[0]-153)<50&&abs(pos_temp[1]-431)<50) //if the temperature down is pressed
			{
				tt--; //NUMBER OF SETPOINT IS DECREASE
				if(tt < 100) // IF THE NUMBER OF SETPOINT EQUAL TO 200 THEN THE SETPOINT GO TO THE 100
				 {
					tt = 200;
				 }
				 delay_ms(200);
				refresh_temp_show = 1;
			}
			if(abs(pos_temp[0]-217)<50&&abs(pos_temp[1]-277)<50) //if the run button is pressed
		 	{
				tempSetPoint = tt;
				runStop = 1;
				refresh_all=1;//refresh indicator
			}
			if(abs(pos_temp[0]-227)<50&&abs(pos_temp[1]-146)<50) //IF THE TOUCHSCREEN IS forward PRESSED TURN ON THE RELAY1
			 {
				forwardReverse = 1;
				GPIOG->BSRRL |=(1 << 8);
				GPIOG->BSRRH =(1 << 6);
				 refresh_all = 1;//refresh all
			 }
			if(abs(pos_temp[0]-120)<50&&abs(pos_temp[1]-145)<50)//IF THE TOUCHSCREEN IS reverse PRESSED TURN ON THE RELAY2
			{
				 forwardReverse = 2;
				 GPIOG->BSRRL |= (1 << 6); 
				 GPIOG->BSRRH |= (1 << 8);				 
					refresh_all = 1;//refresh all
			}
			if(abs(pos_temp[0]-60)<50 && abs(pos_temp[1]-46)<50) //SETTING BUTTON
				{
					state = STATE_SETTING;
					refresh_all = 1;
				}
				if(abs(pos_temp[0]-120)<50 && abs(pos_temp[1]-272)<50) //stop button
				{
					runStop = 0;
					refresh_all = 1;//refresh indicator
					//state = STATE_SETTING;
					//state_switch = 1;
				}
			}
			//general
			// temperature sensor config
			/*Presence = DS18B20_Start ();
			delay_ms(50);
			DS18B20_Write (0xCC);	// skip ROM
			DS18B20_Write (0x44);	// convert t
			delay_ms(50);
			Presence = DS18B20_Start();
			delay_ms(50);
			DS18B20_Write (0xCC);	// skip ROM
			DS18B20_Write (0xBE);	// Read Scratch-pad

			Temp_byte1 = DS18B20_Read();
			Temp_byte2 = DS18B20_Read();
			TEMP = (Temp_byte2<<8)|
			Temp_byte1;
			Temperature	= TEMP/16;
			delay_ms(50);
			//LCD_ShowString(360,300,16,"TEMP:" ,1);
			LCD_ShowNum(300+100,270,Temperature,3,16); // show the number of current temprature (DS18B20)
			LCD_ShowString(40+150,400,16, "^C" ,1);
			delay_ms(100);*/

				
				
				
				
				
				
				
			}
			if(state==STATE_SETTING)
			{
				//display
				if( refresh_all == 1)
				{
				 u16 reversetime = 0.0;
				 LCD_Clear(LIGHTBLUE);
				 Gui_Drawbmp18(20 , 10 , gImage_home33s); //DRAW THE PICTURE
				 LCD_ShowString(120 , 70 ,16 , "REVERSE TIME" , 1); //show reverse time
				 LCD_ShowNum(220+10 , 69 , reversetime , 15 , 15);
				 LCD_ShowString(120 , 100 ,16 ,"FORWARD TIME",1);
				 LCD_ShowNum(220+10 , 100 , reversetime , 15 , 15);				
				 LCD_ShowString(120 , 130 ,16 ,"STAND BY TIME",1);
				 LCD_ShowNum(220+10 , 130 , reversetime , 15 , 15);
				 LCD_ShowString(120 , 160 ,16 ,"WORK TEMP",1);
				 LCD_ShowNum(220+10 , 160 , reversetime , 15 , 15);
				 LCD_ShowString(120 ,190 ,16 ,"OFFSET TEMP",1);
				 LCD_ShowNum(220+10 , 190 , reversetime , 15 , 15);
				 LCD_ShowString(120 , 220 ,16 ,"STAND BY TEMP",1);
				 LCD_ShowNum(220+10 , 220 , reversetime , 15 , 15);
				  refresh_all=0;
				}
				//touch
				if((tp_dev.sta&0xc0) == TP_CATH_PRES)//The button was pressed once (the button is now released.)
				{
					 pos_temp[0] = tp_dev.x;
					 pos_temp[1] = tp_dev.y;
								 if(abs(pos_temp[0]-240)<50&&abs(pos_temp[1]-40)<50)//home button
									{	 
										state=STATE_HOME;
										refresh_all = 1;
									}
					
				}
				
				//general
							
			}
			// runs in overall while loop
			
				if(runStop==1) // if setpoint set on Setpoint
				{
					if(Temperature <= tempSetPoint-10) // check if the temprature is lower than setpoint
					{
						GPIOG->BSRRL |= (1<<14); // turn on the relay
						if(Temperature > tempSetPoint-10) // if temprature is equal to 50
						{
							GPIOG->BSRRH |= (1<<14); // turn of the ssr relay
							delay_ms(5000);
							if(Temperature != tt) // again check if the temprature is not equal to setpoint
							{
								GPIOG->BSRRL |= (1<<14); // turn on relay
								delay_ms(6000);
								GPIOG->BSRRH |= (1<<14); //turn off relay
								delay_ms(2000);
								GPIOG->BSRRL |= (1<<14); // turn on relay
								delay_ms(6000);
								GPIOG->BSRRH |= (1<<14); // turn off relay
								delay_ms(2000);
 								GPIOG->BSRRL |= (1<<14); // turn on relay
								delay_ms(6000);
								GPIOG->BSRRH |= (1<<14); //turn off relay
								delay_ms(2000);
								GPIOG->BSRRL |= (1<<14); // turn on relay
								delay_ms(6000);
								GPIOG->BSRRH |= (1<<14); // turn off relay
								delay_ms(2000);
							}
						}
						if(Temperature >= tempSetPoint)
						{
							GPIOG->BSRRH |= (1<<14); // turn off relay
						}
					}
				}
				
		 if(!(GPIOB->IDR &(1<<9))) // IF THE OCULAR SENSOR IS SENSE TURN ON THE RELAY FOR MOTOR DC AND TURN LENF THE MOTOR
		 {
		 while(!(GPIOB->IDR &(1<<9))); //IF USER DOESN'T PRESS THE TOUCH EVERYSECOND DON'T TURN ON RELAY
		 GPIOG->BSRRL |= (1 << 8); //TURN ON RELAY1
		 GPIOG->BSRRH	= (1 << 6); //TURN OFF RELAY2
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
void configGPIOB (void)
{
	RCC->AHB1ENR |= (1<<1);
	GPIOB->MODER &= ~((1<<18) | (1<<19));
	GPIOA->PUPDR |= (1<<18);
	GPIOG->OTYPER = 0;
	GPIOG->OSPEEDR = 0; 
}



