
/**
* AUTHOR :  POORIYA_ADH   TFT_LCD PROJECT (2022)
 */

#include 	 <temperatureTest.h>
/* Include my libraries here */
#include   "defines.h"
#include   "tm_stm32f4_delay.h"
#define   USE_PID    0
typedef enum
{
	STATE_HOME,
	STATE_SETTING,
	STATE_PASSWORD
}GUI_STATE;
void SetAsOutput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	GPIOx->MODER |=  (1<<2*GPIO_Pin);
	GPIOC->OTYPER &= ~(1<<GPIO_Pin);
  GPIOC->OSPEEDR |= (1<<(2*GPIO_Pin + 1));
}
void SetAsInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	GPIOC->MODER &= ~((1<<2*GPIO_Pin) | (1<<(2*GPIO_Pin+1)));
}
float PIDcontroller(float Temperature,float setPoint,float offset) // TODO
{
	if(Temperature<setPoint-offset)
	{
		return 1.0;
	}
	else
	{
		return ((setPoint - Temperature)/offset);
	}
}


void TestTemperature(void)
{
			float Temperature;
			float TemperaturePrev;
			float tempOffset = 0;
			MAX31865_GPIO max_gpio;
			
	   //relay job variable
			uint32_t  start_time0;
			int       readTime0 = 0;
			uint32_t  duration0 = RELAY_TIME_ON;  //millisecond on time
	
			uint32_t  start_time1;
			uint32_t  duration1 = RELAY_TIME_OFF; //millisecond off time
			int       readTime1 = 0;
			
			
			int      stageRelay = 0;   //0 to 3
	   // temp read job varaibale
		 uint8_t buffer[2];
		 uint16_t data;  
		 float resistance;
		//float temp;
	    uint32_t    start_timetemp0;
			int         readTimetemp0 = 0;
	    uint32_t    durationtemp0 = 10; //millisecond on time

      uint32_t    start_timetemp1;
			int         readTimetemp1 = 0;
	    uint32_t    durationtemp1 = 65;  //millisecond on time
			 
			uint32_t    start_timetemp2;
			int         readTimetemp2 = 0;
	    uint32_t    durationtemp2 = 0;   //millisecond on time
			int         stageTemp = 0;   // 0 to 2 
			
			//pressing temp
			uint32_t    pressTime;
			uint32_t    start_timePress;
			uint32_t    durationPress = 500;//millisecond on time
			 
			//MOTOR

				int stageFORWARDMOTOR1 = 1;//STARTS FROM OFF STAGE
 	      int stageforwardmotortime1 = 0;
				int stageREVERSEMOTOR = 0;
				int stageREVERSEMOTORRIME1 = 0;
				int previousPushButtonState = 0;
				int debouncePushButtonCounter = 0;
				
				//stage test for standbytemp
				int STAGEFORAGAIN = 1;
				int STAGESTB = 0;
				int durationforstandbytime0;
				int durationstandbytimnumber = 0;
				
				uint32_t   start_timemotor1;
			  uint32_t   start_timemotor2;
			 u8	 key;	
			 u16	tt = 50; //FIRST THE VARIABLE OF SETPOINNT IS 50
			 u16	tempSetPoint = 100; //AT THE END WE PUT THE NUMBER FOR THE TT TO TRMPSETPOINT
			 int runStop = 0; // WE CHECK THE STATE IF THE BUTTON IS PRESSED THEN CHANGE THE NUMBER OF RUNSTOP INTO 1 AND DO STH
			 int forwardReverse = 0;  //0 means nothing 1 is forward 2 is reverse motion
			 u16	pp = 10;
			 volatile int	i;
			 volatile int	j; 
			 volatile GUI_STATE state;
			 int refresh_all = 1;
			 int refresh_temp_show = 1;
			 int refresh_temp_set = 0;
			 int start_timer = 1;
			 volatile int timer_counter = 0;
       static u8   fac_us = 168/8;
       static u16  fac_ms = (u16)(168/8)*1000;
			 int   refreshpassword = 0;
			 int   pass1 = 10;
       int   pass2 = 10;
       int   pass3 = 10;
       int   shownumberpass1 = 1;
			 int   shownumberpass2 = 1;
			 int   shownumberpass3 = 1;
       int  reversetimeNUM = 1;
       int reversetimeNUMBER	= 1;
			 int forwardtimenum = 1;
			 int forwardtimeNUMBER = 1;
			 int offsettimeNUM    = 1;
			 int offsettimeNUMBER = 1;
			
			int stagestandbytemp5 = 0;
		  
			//int temp;
		  char str[17];
			uint32_t  durationMOTOR0 = 0;
			uint32_t  durationMOTOR1 = 0;
			//standby variable
		  uint32_t	standbyDuration = 10;
			uint32_t	startStandbyTime = 0;
			int standByTimeRead = 0;
			int standByState = 0;
			
			//int for consstant temp
			int stageconstanttemp = 0; //0 to 1
			int readtimeconstanttemp0 = 0;
		  int 	readtimeconstanttemp1 = 0;
			
			int STANDBYTIMENUMBER = 1;
			int STAGESTANDBYTIME = 1;
			int STANDBYTEMP = 100;
			int refreshForwardTime = 1;
			int refreshOffsetTemp = 1;
			int refreshStandByDuration = 1;
			int	refreshStandbyTemp = 1;
			
			
			u16	pos_temp[2]; //used for storing x y position
			state = STATE_HOME;		
			tp_dev.sta	= 0;	//Eliminate the trigger signal
			tp_dev.xfac = 0;	//xfacIt is used to mark whether it has been calibrated, so it must be cleared before calibration! To avoid errors 
			configpioG(); //config the GPIOG clock
			configGPIOB(); // config the GPIOB clock	
			GPIOG->BSRRL = (1 << 6); // set the pin low 
			GPIOG->BSRRL |= (1 << 8);	// set the pin as low 
			TP_Init();
			//timer init
		
		/* Initialize system */
			SystemInit();
    
		/* Initialize delay */
			TM_DELAY_Init();
    
		/* Reset counter to 0 */
			TM_DELAY_SetTime(0);
		/* max init */
			 SetAsInput(GPIOC, 8);
			 SetAsOutput(GPIOC, 7);
			 SetAsOutput(GPIOC, 12);
			 SetAsOutput(GPIOC, 10);
		   
	
		  
		  //Define SPI pinout
       max_gpio.MISO_PIN  =   GPIO_Pin_8;  //GPIOA->BSRR |= (1 << 6); 
       max_gpio.MISO_PORT =   GPIOC;
       max_gpio.MOSI_PIN  =   GPIO_Pin_7;  //GPIOA->BSRR |= (1 << 7); 
       max_gpio.MOSI_PORT =   GPIOC;
       max_gpio.CLK_PIN   =   GPIO_Pin_12;  //GPIOA->BSRR |= (1 << 5);  
       max_gpio.CLK_PORT  =   GPIOC;
       max_gpio.CE_PIN    =   GPIO_Pin_10; //GPIOA->BSRR |= (1 << 4);   
       max_gpio.CE_PORT   =   GPIOC;
			 MAX31865_init(&max_gpio, 2); 
			 
			 //end of max initialization
			 
					
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
			
			 
    // STATE HOME
			if(state == STATE_HOME)
			{
				//draw
				if(refresh_all==1)
				{		
				LCD_direction(1); // THIS IS THE ROTATION FUNCTION FOR LCD  
				LCD_Clear(LIGHTBLUE); // THE BACKGROUND COLOR IN LCD
				Gui_Drawbmp18	(120	, 70 ,	gImage_FORWARDTRUE); //DRAW THE FORWARD PICTURE
				Gui_Drawbmp18	(20 , 10 ,		gImage_home33s); //DRAW THE PICTURE
				Gui_Drawbmp18	(20	, 220 ,	  gImage_qq_logo); //DRAW THE PICTURE
				Gui_Drawbmp18	(250 , 80,		gImage_run33s); //DRAW THE PICTURE
				Gui_Drawbmp18	(120 , 170 ,	gImage_reverse32s); //DRAW THE PICTURE
				Gui_Drawbmp18	(250	, 170,	gImage_stop33s); //DRAW THE PICTURE		
				Gui_Drawbmp18	(380	, 220,	gImage_temp33s); //DRAW THE PICTURE	 
				Gui_Drawbmp18	(388	, 50 ,	gImage_UPOEEE); //temp up
				Gui_Drawbmp18	(388	, 133 ,	gImage_bachupp); //temp down
				LCD_ShowString (250,50,16," SET:", 1);
				LCD_ShowNum(250+50,50,tt,4,16); //show the number of setpoint temperature
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
	  if(!(tp_dev.sta & TP_PRES_DOWN))	//IF THE TOUCH SCREEN IS NOT PRESSED DOWN
		{
			start_timePress = TM_DELAY_Time(); // THIS IS THE TIME WHEN THE USER DOESN'NT TOUCH THE LCD 
			//GPIOG->BSRRL |=(1 << 8);
			//GPIOG->BSRRL |=(1 << 6);
		}
		if(tp_dev.sta & TP_PRES_DOWN)	//IF THE TOUCH SCREEN IS PRESSED DOWN
		{
			pos_temp[0] = tp_dev.x;
			pos_temp[1] = tp_dev.y;
			
			if(TM_DELAY_Time()-start_timePress>durationPress) 
			{
				if(abs(pos_temp[0]-244)<50&&abs(pos_temp[1]-418)<50)	//if the temperature up is pressed
			 {				
				tt++; //THE NUMBER OF SETPOINT INCREASE
				if(tt > TEMP_HIGH) // IF THE NUMBER OF SETPOINT EQUAL TO 200 THEN THE SETPOINT GO TO THE 100
				 {
					tt = TEMP_LOW;
				 }
				 tempSetPoint = tt;
				 delay_ms(60);
				 refresh_temp_show = 1;
			 }
			 if(abs(pos_temp[0]-153)<50&&abs(pos_temp[1]-431)<50) //if the temperature down is pressed
			{
				tt--; //NUMBER OF SETPOINT IS DECREASE
				if(tt < TEMP_LOW) // IF THE NUMBER OF SETPOINT EQUAL TO 200 THEN THE SETPOINT GO TO THE 100
				 {
					tt = TEMP_HIGH;
				 }
				 tempSetPoint = tt;
				 delay_ms(60);
				refresh_temp_show = 1;
			}
			if(abs(pos_temp[0]-227)<50&&abs(pos_temp[1]-146)<50) //IF THE TOUCHSCREEN IS forward PRESSED TURN ON THE RELAY1
			 {
				forwardReverse = 1;
				GPIOG->BSRRL |=(1 << 8);
				GPIOG->BSRRH =(1 << 6);
				 //refresh_all = 1;//refresh all
			 }
			  if(abs(pos_temp[0]-120)<50&&abs(pos_temp[1]-145)<50)//IF THE TOUCHSCREEN IS reverse PRESSED TURN ON THE RELAY2
			{
				 forwardReverse = 2;
				 GPIOG->BSRRL |= (1 << 6); 
				 GPIOG->BSRRH |= (1 << 8);				 
					//refresh_all = 1;//refresh all
			}
			
			}
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
				if(tt > TEMP_HIGH) // IF THE NUMBER OF SETPOINT EQUAL TO 200 THEN THE SETPOINT GO TO THE 100
				 {
					tt = TEMP_LOW;
				 }
				 tempSetPoint = tt;
				 delay_ms(200);
				 refresh_temp_show = 1;
			 }
			 if(abs(pos_temp[0]-153)<50&&abs(pos_temp[1]-431)<50) //if the temperature down is pressed
			{
				tt--; //NUMBER OF SETPOINT IS DECREASE
				if(tt < TEMP_LOW) // IF THE NUMBER OF SETPOINT EQUAL TO 200 THEN THE SETPOINT GO TO THE 100
				 {
					tt = TEMP_HIGH;
				 }
				 tempSetPoint = tt;
				 delay_ms(200);
				refresh_temp_show = 1;
			}
			if(abs(pos_temp[0]-217)<50&&abs(pos_temp[1]-277)<50) //if the run button is pressed
		 	{
				tempSetPoint = tt;
				runStop = 1;
				refresh_all = 1;   //refresh indicator
			}
	    
			//this stage is for PASSWORD STAGEE	
			if(abs(pos_temp[0]-60)<50 && abs(pos_temp[1]-46)<50) //SETTING BUTTON
				{
					state = STATE_PASSWORD;
					refreshpassword = 1;
				}
				if(abs(pos_temp[0]-120)<50 && abs(pos_temp[1]-272)<50) //stop button
				{
					runStop = 0;
					refresh_all = 1;//refresh indicator
				}
			}
			
			
		//***************************************//
			//THIS STAGE IS FOR DS18B20 SENSOR IF YOU WANT TO USE THIS UNCOMMENT THI CODE
		//***************************************//	
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
			
			}//END OF HOME
	  
		
				
				//password  STATE
			 if(state == STATE_PASSWORD)
			   {
			  	if(refreshpassword == 1)
			  	{
				  LCD_Clear(LIGHTBLUE); // BACKGROUND COLOR FOR LCD
					Gui_Drawbmp18	(388	, 50 ,	gImage_enterbutton);
		      LCD_ShowString(150 , 70 ,16 , "ENTER THE PASSWORD" , 1);
          refreshpassword = 0;					
				  }
					if(shownumberpass1 == 1) // SHOW PASSWORD 1
					{
			   	LCD_ShowNum(70+20,200,pass1,4,16);
					shownumberpass1 = 0;
					}
					if(shownumberpass2 == 1) // SHOW PASSWORD 2
					{
						LCD_ShowNum(150+50,200,pass2,4,16);
					}
					if(shownumberpass3 == 1)
					{
						LCD_ShowNum(250+50,200,pass3,4,16); // SHOW PASSWORD 3
					}
					if((tp_dev.sta&0xc0) == TP_CATH_PRES)//The button was pressed once (the button is now released.)
				   {
					 pos_temp[0] = tp_dev.x;
					 pos_temp[1] = tp_dev.y;
					if(abs(pos_temp[0]-240)<50&&abs(pos_temp[1]-40)<50)  //home button
					 {	 
					   state  = STATE_HOME;
						 refresh_all = 1;
					 }	 
				if(abs(pos_temp[0]-132)<50&&abs(pos_temp[1]-153)<50)	//if the pass1 button is pressed increase the password
			  {				
				pass1++; //THE NUMBER OF PASSWORD INCREASE
				if(pass1 > 40) // IF THE NUMBER OF SETPOINT EQUAL TO 200 THEN THE SETPOINT GO TO THE 100
				 {
					pass1 = 10;
				 }
				 //tempSetPoint = tt;
				  delay_ms(50);
				  shownumberpass1 = 1;
			   }
			if(abs(pos_temp[0]-86)<50&&abs(pos_temp[1]-150)<50)	//if the  PASSWORD DOWN is pressed
			  {				
				  pass1--; //THE NUMBER OF SETPOINT INCREASE
				if(pass1 <= 0) // IF THE NUMBER OF SETPOINT EQUAL TO 200 THEN THE SETPOINT GO TO THE 100
				 {
					pass1 = 10;
				 }
				 //tempSetPoint = tt;
				  delay_ms(50);
				  shownumberpass1 = 1;
			   }
				if(abs(pos_temp[0]-140)<50&&abs(pos_temp[1]-255)<50)   //if the  PASSWORD2 UP is pressed
				{
					pass2++;
					if(pass2 > 40)
					{
						pass2 = 10;
					}
					delay_ms(50);
					shownumberpass2 = 1;;
				}
				if(abs(pos_temp[0]-91)<50&&abs(pos_temp[1]-264)<50) //if the  PASSWORD2 DOWN is pressed
				{
					pass2--;
					if(pass2 <= 0)
					{
						pass2 = 10;
					}
					delay_ms(50);
					shownumberpass2 = 1;;
				}
		   if(abs(pos_temp[0]-154)<50&&abs(pos_temp[1]-360)<50)  //if the  PASSWORD3 UP is pressed
		    {
					pass3++;
					if(pass3 >= 40)
					{
						pass3 = 10;
					}
			    delay_ms(50);
					shownumberpass3 = 1;
		    }
				 if(abs(pos_temp[0]-77)<50&&abs(pos_temp[1]-362)<50) //if the  PASSWORD3 DOWN is pressed
		    {
					pass3--;
					if(pass3 <= 0)
					{
						pass3 = 10;
					}
			    delay_ms(50);
					shownumberpass3 = 1;
		    }
				if(abs(pos_temp[0]-272)<50&&abs(pos_temp[1]-413)<50) 
				{
					if(pass1 ==  NUMBERPASS1 && pass2 ==  NUMBERPASS2 && pass3 ==  NUMBERPASS3)
					{
						state = STATE_SETTING;
						refresh_all = 1;
					}
		     if(pass1 !=  NUMBERPASS1 || pass2 !=  NUMBERPASS2 || pass3 !=  NUMBERPASS3)
					{
					LCD_ShowString(190 , 20 ,20 ,"INCORRECT PASSWORD TRY AGAIN",1);
					}
				}
			 }		
	  } // END OF PASSWORD STATE
				 
				 
			if(state == STATE_SETTING)
			{
				//display
				if( refresh_all == 1)
				{
				  u16 reversetime = 0;
				  LCD_Clear(LIGHTBLUE);
				  Gui_Drawbmp18(20 , 10 , gImage_home33s); //DRAW THE PICTURE
				  LCD_ShowString(50 ,150 ,16 , "FORWARD TIME",1);				
			   LCD_ShowString(120 , 70 ,16 ,"STAND BY TIME",1);
				  LCD_ShowString(50 ,240 ,16 ,"OFFSET TEMP",1);
				  LCD_ShowString(300 , 150 ,16 ,"STANDBYTEMP",1);
				  refresh_all=0;
				}
				if(refreshForwardTime == 1)
				{
					LCD_ShowNum(150+10,150, forwardtimeNUMBER ,15,15);
					refreshForwardTime = 0;
				}
				
				if(refreshOffsetTemp == 1)
				{
					LCD_ShowNum(150+10 , 240 , offsettimeNUMBER, 15 , 15);
					refreshOffsetTemp = 0;
				}
				if(refreshStandByDuration == 1)
				{
					LCD_ShowNum(220+10 , 69 , STANDBYTIMENUMBER , 15 , 15);
					refreshStandByDuration = 0;
				}
				if(refreshStandbyTemp == 1)
				{
					LCD_ShowNum(380+10 , 150 , STANDBYTEMP, 12 , 12);
					refreshStandbyTemp = 0;
				}
				
				//touch
				if((tp_dev.sta&0xc0) == TP_CATH_PRES)//The button was pressed once (the button is now released.)
				{
					 pos_temp[0] = tp_dev.x;
					 pos_temp[1] = tp_dev.y;
			if(abs(pos_temp[0]-303)<50&&abs(pos_temp[1]-43)<50)//home button
					{	 
							state = STATE_HOME;
							refresh_all = 1;
					}
					if(abs(pos_temp[0]-110)<50&&abs(pos_temp[1]-77)<50)  //OFFSET Temp plus IN SETTING BUTTON
				  {					  
						  offsettimeNUMBER++;
						  if(offsettimeNUMBER == 16)
						  {
							 offsettimeNUMBER = 0;
						  }
							refreshOffsetTemp = 1;
						  delay_ms(50);
					}
					if(abs(pos_temp[0]-120)<50&&abs(pos_temp[1]-191)<50) //OFFSET Temp minus IN SETTING BUTTON
				  {
						 offsettimeNUMBER--;
						 if(offsettimeNUMBER <= -16)
						 {
							 offsettimeNUMBER = -16;
						 }
						 refreshOffsetTemp =1;
						 delay_ms(50);
					}
					
					if(abs(pos_temp[0]-207)<50&&abs(pos_temp[1]-65)<50)//FORWARD TIME plus IN SETTING BUTTON
				  {
						 
						 forwardtimeNUMBER++;
						 if(forwardtimeNUMBER > 41)
						 {
							forwardtimeNUMBER = 0;
						 }
						 refreshForwardTime = 1;
						 delay_ms(100);
					}
					if(abs(pos_temp[0]-215)<50&&abs(pos_temp[1]-238)<50)//FORWARD TIME minus IN SETTING BUTTON
				  { 
						 forwardtimeNUMBER--;
						 if(forwardtimeNUMBER <=  0)
						 {
							forwardtimeNUMBER = 0;
						 }
						 refreshForwardTime = 0;
						 delay_ms(100);
					}
					
					// OFFSET TEMP SET button
					if(abs(pos_temp[0]-80)<50&&abs(pos_temp[1]-300)<50) // OFFSET TEMP 
					{
						tempOffset = offsettimeNUMBER;
						
					}
				if(abs(pos_temp[0]-283)<50&&abs(pos_temp[1]-159)<50)//  THE TEST STAND BY TIME
				 {
					 
					 STANDBYTIMENUMBER++;
					 if(STANDBYTIMENUMBER == 11)
					 {
						 STANDBYTIMENUMBER = 1;
					 }
					 refreshStandByDuration = 1;
					 
				 }
				 if(abs(pos_temp[0]-284)<50&&abs(pos_temp[1]-301)<50)  //  THE TEST STAND BY TIME
				 {
					 
					 STANDBYTIMENUMBER--;
           if(STANDBYTIMENUMBER <= 1)
					  {
						 STANDBYTIMENUMBER = 1;
					  }					 
					refreshStandByDuration = 1;
				 }
				 if(abs(pos_temp[0]-200)<50&&abs(pos_temp[1]-318)<50) //  THE TEST STAND BY TEMP
				 {
					 
					 STANDBYTEMP++;
					 if(STANDBYTEMP >= 180)
					 {
						 STANDBYTEMP = 100;
					 }
					 refreshStandbyTemp = 1;
				 }
				 if(abs(pos_temp[0]-200)<50&&abs(pos_temp[1]-432)<50) //  THE TEST STAND BY TEMP
				 {
					 
					 STANDBYTEMP--;
					 if(STANDBYTEMP <= 100)
					 {
						 STANDBYTEMP = 100;
					 }
					 refreshStandbyTemp = 1;
				 }
				}
				//general
							
			}//END OF SETTING
			                 
	                       		
		                    	
			                 
			           
	  //temperature sensor config  DS18B20 WITH TIMER INIT
	  /*if(stageTemp == 0)
		  {
		  //GPIOG->BSRRL |= (1<<14); // turn on the relay
			if(readTimetemp0 == 0)
			{
			start_timetemp0 = TM_DELAY_Time();
			//job
      Presence = DS18B20_Start ();
				
			readTimetemp0 = 1;
			}
			
			if(TM_DELAY_Time()-start_timetemp0>durationtemp0)
			stageTemp = 1;
		}
			
		if(stageTemp==1)
		{
		//	GPIOG->BSRRL |= (1<<14); // turn on the relay
			if(readTimetemp1 == 0)
			{
			start_timetemp1 = TM_DELAY_Time();
				
			//job
      DS18B20_Write (0xCC);	// skip ROM
			DS18B20_Write (0x44);	// convert t
				
			readTimetemp1 = 1;
			}
			
			if(TM_DELAY_Time()-start_timetemp1>durationtemp1)
			stageTemp = 2;
				
		}
		
			if(stageTemp==2)
		{
		//	GPIOG->BSRRL |= (1<<14); // turn on the relay
			if(readTimetemp2 == 0)
			{
			start_timetemp2 = TM_DELAY_Time();
			
			//job
      	Presence = DS18B20_Start();
				
			readTimetemp2 = 1;
			}
			
			
			if(TM_DELAY_Time()-start_timetemp2>durationtemp2)
			stageTemp = 3;
				
		}
		
	if(stageTemp==3)
		{
		//	GPIOG->BSRRL |= (1<<14); // turn on the relay
			if(readTimetemp3 == 0)
			{
			start_timetemp3 = TM_DELAY_Time();
			
			//job
    	DS18B20_Write (0xCC);	// skip ROM
			DS18B20_Write (0xBE);	// Read Scratch-pad

			Temp_byte1 = DS18B20_Read();
			Temp_byte2 = DS18B20_Read();
			TEMP = (Temp_byte2<<8)|
			Temp_byte1;
			Temperature	= TEMP/16;
				
			readTimetemp3= 1;
			}
			
			if(TM_DELAY_Time()-start_timetemp3>durationtemp3)
			stageTemp = 4;
				
		}
		
			if(stageTemp==4)
		{
		//	GPIOG->BSRRL |= (1<<14); // turn on the relay
			if(readTimetemp4 == 0)
			{
			start_timetemp4 = TM_DELAY_Time();
			
			//job
				if(state==STATE_HOME)
      	LCD_ShowNum(300+100,270,Temperature,3,16);
				
			readTimetemp4 = 1;
			}
			
			
			if(TM_DELAY_Time()-start_timetemp4>durationtemp4)
			{
			  stageTemp = 0;
				readTimetemp0 = 0;
				readTimetemp1 = 0;
				readTimetemp2 = 0;
				readTimetemp3 = 0;
				readTimetemp4 = 0;
			}		
				
		*///} // END OF SENSOR CONFIG DS18B20
	
				//*********************************//
				//THIS IS FOR MAX31865 FOR PT100 CONFIG
				//********************************//
			 //Temperature = MAX31865_readTemp();
			 //READ TEMPERATURE
			if(stageTemp == 0)
		  {
		  //GPIOG->BSRRL |= (1<<14); // turn on the relay
			if(readTimetemp0 == 0)
			{
			start_timetemp0 = TM_DELAY_Time();
			//job
      enableBias(ON);
			
			readTimetemp0 = 1;
			}
			
			if(TM_DELAY_Time()-start_timetemp0>durationtemp0)
			stageTemp = 1;
				
			}
			if(stageTemp == 1)
		  {
		  //GPIOG->BSRRL |= (1<<14); // turn on the relay
			if(readTimetemp1 == 0)
			{
			start_timetemp1 = TM_DELAY_Time();
			//job
      single_shot();
			
			readTimetemp1 = 1;
			}
			
			if(TM_DELAY_Time()-start_timetemp1>durationtemp1)
			stageTemp = 2;
				
			}
			if(stageTemp == 2)
		  {
			if(readTimetemp2 == 0)
			{
			start_timetemp2 = TM_DELAY_Time();
			//job
		
      // Read data from max31865 data registers
    MAX31865_read(MAX31856_RTDMSB_REG, buffer, 2);

    // Combine 2 bytes into 1 number, and shift 1 down to remove fault bit
    data = buffer[0] << 8;
    data |= buffer[1];
    data >>= 1;

    // Calculate the actual resistance of the sensor
     resistance = ((float) data * RREF) / FACTOR;

    // Calculate the temperature from the measured resistance
     Temperature = ((resistance / 100) - 1) / ALPHA - tempOffset;
		//Temperature = temp;
    // Disable bias voltage to reduce power usage
			enableBias(OFF);	
			readTimetemp2 = 1;
			}
			
			if(TM_DELAY_Time()-start_timetemp2>durationtemp2)
			{
				readTimetemp0 = 0;
				readTimetemp1 = 0;
				readTimetemp2 = 0;
				stageTemp = 0;
			}
		}
			//end of read temperature MAX31865
			
			 
			 // THIS STATE FOR SHOWING THE TEMPRATURE IN HOME STATE
			 if(state==STATE_HOME)
			 {
				 char number[6]="      ";
				 int end;
				 float namayesh = Temperature;
				 //Temperature =150.15; 
				 if(Temperature>tt-5&&Temperature<tt+5)
				 {
					 namayesh = tt;
				 }
				 else if(Temperature>STANDBYTEMP-5 &&Temperature<STANDBYTEMP+5 )
				 {
					 namayesh = STANDBYTEMP;
				 }
				 else if(USE_PID)
					 namayesh=Temperature;
				 //sprintf(number,"%3.1f",tt);
					 
				 sprintf(number,"%3.1f",namayesh);
				 end = namayesh>=100?5:4;
				 for (i=0;i<end;i++)
				 LCD_ShowChar(400+i*8,270,POINT_COLOR,BACK_COLOR,(u8)number[i],16,0);
			 delay_ms(1);
			 }
			 // END OF THE TEMPRATURE SHOW IN STATE HOME
			 
			 
			 //ANOTHR TEMPRATURE SHOW IN SETTING IF YOU WANT YOU CAN USE IT
			 if(state==STATE_SETTING)
			 {
				 char number[6]="      ";
				 int end;
				 //Temperature =150.15; 
				 sprintf(number,"%3.1f",Temperature);
				 end = Temperature>=100?5:4;
				 for (i=0;i<end;i++)
				 LCD_ShowChar(400+i*8,270,POINT_COLOR,BACK_COLOR,(u8)number[i],16,0);
			   delay_ms(1);
			  } // END OF TEMPRATURE SHOW IN SETTIN STATE
			 
			 
		  // runs in overall while loop
				if(runStop == 1) // if setpoint set on Setpoint
				{
					if(USE_PID == 1)
					{
						duration0 = PID_DUTY_CYCLE*1000*PIDcontroller(Temperature,tempSetPoint,PID_OFFSET);
						duration1 = PID_DUTY_CYCLE*1000 - duration0;
					}
					
					/*if(TM_DELAY_Time()-startStandbyTime>standbyDuration)
					{
						standByState = 1;
						tempSetPoint = STANDBYTEMP;
					}
					else
					{
						standByState = 0;
						tempSetPoint = tt;
					*///}
					
					if(Temperature <= tempSetPoint-TEMP_DIFF) // check if the temprature is lower than setpoint minus ten
					{
						GPIOG->BSRRL |= (1<<14); // turn on the relay
					}
					if(Temperature > tempSetPoint-TEMP_DIFF&&Temperature < tempSetPoint) // check if the temprature is high than setpoint minus
					{	
						if(stageRelay==0)
						{
						//	GPIOG->BSRRL |= (1<<14); // turn on the relay
							if(readTime0 == 0)
							{
							start_time0 = TM_DELAY_Time();
			
							//job
							GPIOG->BSRRL |= (1<<14); // turn on the relay
				
							readTime0 = 1;
							}
			
			         if(TM_DELAY_Time()-start_time0>duration0)
							 stageRelay = 1;
						}
						if(stageRelay==1)
						{
					    //GPIOG->BSRRL |= (1<<14); // turn on the relay
							if(readTime1 == 0)
							{
							start_time1 = TM_DELAY_Time();
			
							//job
							GPIOG->BSRRH |= (1<<14); // turn off the relay
				
							readTime1 = 1;
							}
							
							if(TM_DELAY_Time()-start_time1>duration1)
							{
							stageRelay = 0;
							readTime0 =  0;
							readTime1 =  0;
							}
						}
		
					}
					if(Temperature >= tempSetPoint)
					{
						GPIOG->BSRRH |= (1 << 14);
					}
					
					
					
					// && !(GPIOB->IDR &(1<<9)) if push button is pressed down
					// push button debounce
					// previous state is zero and current state is one
					//standby
					if(!(GPIOB->IDR &(1<<9)))
					{
						debouncePushButtonCounter++;
						if(debouncePushButtonCounter==10)
						{
							stageFORWARDMOTOR1 = 0;
						}
					}
					else
					{
						debouncePushButtonCounter = 0;
						standbyDuration = STANDBYTIMENUMBER * 60000;
						//durationforstandbytime0 = TM_DELAY_Time();
					}
					if(Temperature > tempSetPoint - 5 &&  TemperaturePrev<=tempSetPoint - 5) 
					{
						standByTimeRead = 1;
					}
					TemperaturePrev = Temperature;
					
					
					
					if(standByTimeRead == 1)
					{
					  startStandbyTime = TM_DELAY_Time();
						standByTimeRead = 0;
					}
					
					
					
					
			
	     if(Temperature > tt - 5 && Temperature < tt + 5) 
		   {
				
				  if(stageFORWARDMOTOR1 == 0)
				   {
					 if(stageforwardmotortime1 == 0)
					 {
						start_timemotor1 = TM_DELAY_Time();
						 //job
						 GPIOG->BSRRL |=(1 << 8);
				     GPIOG->BSRRH =(1 << 6);
						 durationMOTOR0 =  forwardtimeNUMBER * 1000;
						 stageforwardmotortime1 = 1;
					 }
					 	 if(TM_DELAY_Time()-start_timemotor1>durationMOTOR0)
					     stageFORWARDMOTOR1 = 1;
						 if(stageFORWARDMOTOR1 == 1)
						{
							
							GPIOG->BSRRL |=(1 << 8);
				      GPIOG->BSRRL = (1 << 6);
							stageforwardmotortime1 = 0;
							standByTimeRead=1;
							//stageFORWARDMOTOR1 = 0;
						}
				   }
				 }
			 
			   //STAGE FOR STANDBYTIME
			 if(Temperature >= tempSetPoint -5 && Temperature <= tempSetPoint+5 && (GPIOB->IDR &(1<<9)))
			 {
				   if(STAGEFORAGAIN == 1)
					 { 
					  if(STAGESTB == 0)
					  {
				     durationforstandbytime0 = TM_DELAY_Time();
						 //JOB 
						 (GPIOB->IDR & (1<<9)); // HUMAN DOESN'T PUSH THE BUTTON
						durationstandbytimnumber = STANDBYTIMENUMBER * 60000; // set the timee to minute 
						 STAGESTB = 1;
					 }
				 if(TM_DELAY_Time()-durationforstandbytime0 > durationstandbytimnumber)
            STAGEFORAGAIN = 2;
				 
				   if(STAGEFORAGAIN == 2)
              {
               tempSetPoint = STANDBYTEMP;
							 STAGEFORAGAIN = 3;	
							}
							if(STAGEFORAGAIN == 3)
							{
								stagestandbytemp5 = 1;
								STAGEFORAGAIN = 1;
								STAGESTB = 0; 
             }
			    }
			 }
		 }
				
		   if(stagestandbytemp5 == 1)
			{
			   if(Temperature >= STANDBYTEMP - 5 && !(GPIOB->IDR &(1<<9)))
				 {
					 while(!(GPIOB->IDR &(1<<9)));
					 tempSetPoint = tt;
					 stagestandbytemp5 = 0;
				 }
			}
				
				if(runStop == 0)
				{
					GPIOG->BSRRH |= (1<<14); // turn off relay
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
	GPIOB->PUPDR |= (1<<18);
	GPIOB->OTYPER = 0;
	GPIOB->OSPEEDR = 0; 
}

	




