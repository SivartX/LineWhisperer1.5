/*
 * LineWhisperer1.5.c
 *
 * Created: 3/12/2023 7:14:02 PM
 * Author : Bowman
 */ 

#include "C:\Users\Bowman\Documents\Atmel Studio\7.0\MRE_lib\F_cpu_lib.h"
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "C:\Users\Bowman\Documents\Atmel Studio\7.0\MRE_lib\A2D_lib.c"
#include "C:\Users\Bowman\Documents\Atmel Studio\7.0\MRE_lib\A2D_lib.h"
#include "C:\Users\Bowman\Documents\Atmel Studio\7.0\MRE_lib\LCD_lib.h"
#include "C:\Users\Bowman\Documents\Atmel Studio\7.0\MRE_lib\LCD_lib.c"
#include "C:\Users\Bowman\Documents\Atmel Studio\7.0\MRE_lib\Serial_lib2.h"
#include "C:\Users\Bowman\Documents\Atmel Studio\7.0\MRE_lib\Serial_lib2.c"
#include "C:\Users\Bowman\Documents\Atmel Studio\7.0\MRE_lib\Motor_PWM_lib.h"
#include "C:\Users\Bowman\Documents\Atmel Studio\7.0\MRE_lib\Motor_PWM_lib.c"

//************state defines*******************//
#define FAST_FORWARD 001 //state//F
#define SLOW_FORWARD 002 //state//W
#define SOFT_LEFT 003 //state//L
#define SOFT_RIGHT 004 //state//R
#define HARD_LEFT 005 //state//T
#define HARD_RIGHT 006 //state//H
#define STATE_STOP 007 //state//S

//****Directions*****//
#define FORWARD 1
#define	BACKWARDS 0 
//****Speed Values*****//
#define FAST 90
#define SLOW 50
#define STOP 0
//******Motor Drive******//
#define RIGHT_DRIVE 1
#define LEFT_DRIVE 0
//************End state defines*******************//


int main(void)
{
	/*================Start Initialization================*/
	//Define some variables that will be used
	uint16_t adc_val = 0;
	char lcd_str_out[20];
	uint8_t serialInput;
	char serial_char_array_out[20];
	uint8_t LTRIM = 0;
	uint8_t RTRIM = 0;
	//Enable the LED on the microcontroller board for debugging
	DDRC |= 1 << PINC2;
	//Initialize the LCD and send a test message
	LCD_init();
	LCDGoToPosition(1,1);
	sprintf(lcd_str_out,"LOADING...");
	LCDSendString(lcd_str_out);
	_delay_ms(2000);
	AD_init();
	USART_vInit();
	sei();
	LCDClearScreen();
	//Initialize Motor control
	HBridgeInit();
	uint8_t state = STATE_STOP;	
	/*==================End Initialization==================*/
	


	while(1)
	{	
		
			if(serialCheckRxComplete() != 0)
			{
				serialInput = USART_vReceiveByte();
				serialClearBuffer();
			}
			
			LCDClearScreen();
			sprintf(lcd_str_out, "%c",serialInput);
			LCDSendString(lcd_str_out);
			//LCDGoToPosition(1,1);
			
			LCDGoToPosition(14,1);
			sprintf(lcd_str_out,"%.3d",LTRIM);
			LCDSendString(lcd_str_out);
			
			LCDGoToPosition(14,2);
			sprintf(lcd_str_out,"%.3d",RTRIM);
			LCDSendString(lcd_str_out);
			
			//Trims the motor speed to allow the robot to move straight
			if(isdigit(serialInput)){
				
				if(serialInput == '1' && -1 + LTRIM >= 0) --LTRIM;
				else if(serialInput == '2') ++LTRIM;
				if(serialInput == '3' && -1 + RTRIM >= 0) --RTRIM;
				else if(serialInput == '4') ++RTRIM;
				
				serialInput = 'S';
			}
								
			switch(state)
			{

				case FAST_FORWARD:

				HBridgeCommand(LEFT_DRIVE,FAST+LTRIM,FORWARD);
				HBridgeCommand(RIGHT_DRIVE,FAST+RTRIM,FORWARD);

				LCDGoToPosition(1,2);
				sprintf(lcd_str_out,"FAST_FORWARD");
				LCDSendString(lcd_str_out);

				if(serialInput == 'F') {
					state = FAST_FORWARD;
				}
				else if(serialInput == 'W'){
					state = SLOW_FORWARD;
				}
				else if(serialInput == 'L'){
					state = SOFT_LEFT;
				}
				else if(serialInput == 'R'){
					state = SOFT_RIGHT;
				}
				else if (serialInput == 'S'){
					state = STATE_STOP;
				}

				break;

				case SLOW_FORWARD:

				HBridgeCommand(LEFT_DRIVE,SLOW+LTRIM,FORWARD);
				HBridgeCommand(RIGHT_DRIVE,SLOW+RTRIM,FORWARD);

				LCDGoToPosition(1,2);
				sprintf(lcd_str_out,"SLOW_FORWARD");
				LCDSendString(lcd_str_out);

				if(serialInput == 'F'){
					state = FAST_FORWARD;
				}
				else if(serialInput == 'W'){
					state = SLOW_FORWARD;
				}
				else if (serialInput == 'S'){
					state = STATE_STOP;
				}
				else if(serialInput == 'L'){
					state = SOFT_LEFT;
				}
				else if(serialInput == 'R'){
					state = SOFT_RIGHT;
				}

				break;

				case SOFT_LEFT:

				HBridgeCommand(LEFT_DRIVE,FAST+LTRIM,FORWARD);
				HBridgeCommand(RIGHT_DRIVE,SLOW+RTRIM,FORWARD);

				LCDGoToPosition(1,2);
				sprintf(lcd_str_out,"SOFT_LEFT");
				LCDSendString(lcd_str_out);

				if(serialInput == 'F'){
					state = FAST_FORWARD;
				}
				else if(serialInput == 'W'){
					state = SLOW_FORWARD;
				}
				else if(serialInput == 'L'){
					state = SOFT_LEFT;
				}
				else if(serialInput == 'R'){
					state = SOFT_RIGHT;
				}
				else if(serialInput == 'T'){
					state = HARD_LEFT;
				}
				else if(serialInput == 'H'){
					state = HARD_RIGHT;
				}
				else if (serialInput == 'S'){
					state = STATE_STOP;
				}

				break;

				case SOFT_RIGHT:

				HBridgeCommand(LEFT_DRIVE,SLOW+LTRIM,FORWARD);
				HBridgeCommand(RIGHT_DRIVE,FAST+RTRIM,FORWARD);

				LCDGoToPosition(1,2);
				sprintf(lcd_str_out,"SOFT_RIGHT");
				LCDSendString(lcd_str_out);

				if(serialInput == 'F'){
					state = FAST_FORWARD;
				}
				else if(serialInput == 'W'){
					state = SLOW_FORWARD;
				}
				else if(serialInput == 'L'){
					state = SOFT_LEFT;
				}
				else if(serialInput == 'R'){
					state = SOFT_RIGHT;
				}
				else if(serialInput == 'T'){
					state = HARD_LEFT;
				}
				else if(serialInput == 'H'){
					state = HARD_RIGHT;
				}
				else if (serialInput == 'S'){
					state = STATE_STOP;
				}

				break;

				case HARD_LEFT:

				HBridgeCommand(LEFT_DRIVE,FAST+LTRIM,FORWARD);
				HBridgeCommand(RIGHT_DRIVE,STOP,FORWARD);

				LCDGoToPosition(1,2);
				sprintf(lcd_str_out,"HARD_LEFT");
				LCDSendString(lcd_str_out);

				if(serialInput == 'F'){
					state = FAST_FORWARD;
				}
				else if(serialInput == 'W'){
					state = SLOW_FORWARD;
				}
				else if(serialInput == 'L'){
					state = SOFT_LEFT;
				}
				else if(serialInput == 'R'){
					state = SOFT_RIGHT;
				}
				else if(serialInput == 'T'){
					state = HARD_LEFT;
				}
				else if(serialInput == 'H'){
					state = HARD_RIGHT;
				}
				else if (serialInput == 'S'){
					state = STATE_STOP;
				}

				break;

				case HARD_RIGHT:

				HBridgeCommand(LEFT_DRIVE,STOP,FORWARD);
				HBridgeCommand(RIGHT_DRIVE,FAST+RTRIM,FORWARD);

				LCDGoToPosition(1,2);
				sprintf(lcd_str_out,"HARD_RIGHT");
				LCDSendString(lcd_str_out);

				if(serialInput == 'F'){
					state = FAST_FORWARD;
				}
				else if(serialInput == 'W'){
					state = SLOW_FORWARD;
				}
				else if(serialInput == 'L'){
					state = SOFT_LEFT;
				}
				else if(serialInput == 'R'){
					state = SOFT_RIGHT;
				}
				else if(serialInput == 'T'){
					state = HARD_LEFT;
				}
				else if(serialInput == 'H'){
					state = HARD_RIGHT;
				}
				else if (serialInput == 'S'){
					state = STATE_STOP;
				}

				break;

				case STATE_STOP:

				HBridgeCommand(LEFT_DRIVE,STOP,FORWARD);
				HBridgeCommand(RIGHT_DRIVE,STOP,FORWARD);

				LCDGoToPosition(1,2);
				sprintf(lcd_str_out,"STATE_STOP");
				LCDSendString(lcd_str_out);

				if(serialInput == 'W'){
					state = SLOW_FORWARD;
				}
				else if (serialInput == 'S'){
					state = STATE_STOP;
				}
				else if(serialInput == 'F'){
					state = FAST_FORWARD;
				}

				break;

				default: state = STATE_STOP;
				break;
			
				}
	
		for(uint8_t ii =0; ii<20; ii++){
			uart_putchar(serial_char_array_out[ii],&mystdout);
		}
		uart_putchar('\n',&mystdout);
	}
}