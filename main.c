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

//**** Motor Directions*****//
#define FORWARD 1
#define	BACKWARDS 0 
//****Speed Values*****//
#define FAST 90
#define SLOW 40
#define SOFT_TURN 10
#define STOP 0
#define HARD_TURN 5
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
		
		//function called serialCheckRxComplete(). The purpose of this function is likely to check whether there is any available data to be read from the serial port. 
		//The != 0 comparison checks whether the function returns a value other than 0, which determinds that there is data available. Acts as an Interrupt.
			if(serialCheckRxComplete() != 0)
			{
				serialInput = USART_vReceiveByte(); //input from  the higher level of CS. F- FAST_FORWARD, W-SLOW_FORWARD,L-SOFT_LEFT, R-SOFT_RIGHT, T-HARD_LEFT, H- HARD_RIGHT, S-STATE_STOP
				// 1- subtract 1 LEFT TRIM, 2- add 1 LEFT Trim, 3- subtract 1 RIGHT TRIM, 4- add 1 RIGHT Trim,
			}
			
			LCDClearScreen();
			sprintf(lcd_str_out, "%c",serialInput); // Displays the serial data received
			LCDSendString(lcd_str_out);
			
			LCDGoToPosition(14,1);
			sprintf(lcd_str_out,"%.3d",LTRIM);// Displays the left drive trim
			LCDSendString(lcd_str_out);
			
			LCDGoToPosition(14,2);
			sprintf(lcd_str_out,"%.3d",RTRIM);// Displays the right drive trim
			LCDSendString(lcd_str_out);
			
			//Trims the motor speed to allow the robot to move straight
			if(isdigit(serialInput)){
				
				if(serialInput == '1' && -1 + LTRIM >= 0) --LTRIM; // subtracts 1 from the left drive, will not go below zero
				else if(serialInput == '2') ++LTRIM;// add 1 from the left drive
				if(serialInput == '3' && -1 + RTRIM >= 0) --RTRIM; // subtracts 1 from the right drive, will not go below zero
				else if(serialInput == '4') ++RTRIM;// add 1 from the right drive
				
				serialInput = 'S'; //will stop the robot to give time to adjust trim
			}
								
			switch(state)
			{

				case FAST_FORWARD:

				HBridgeCommand(LEFT_DRIVE,FAST+LTRIM,FORWARD); //motor command to the H-bridge, (The motor effected, speed, direction)
				HBridgeCommand(RIGHT_DRIVE,FAST+RTRIM,FORWARD);//motor command to the H-bridge, (The motor effected, speed, direction)

				LCDGoToPosition(1,2); // Displays the current state on the second line of the LCD
				sprintf(lcd_str_out,"FAST_FORWARD");
				LCDSendString(lcd_str_out);
				
				// Allowed state transfer below:

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

				HBridgeCommand(LEFT_DRIVE,SLOW+LTRIM,FORWARD);//motor command to the H-bridge, (The motor effected, speed, direction)
				HBridgeCommand(RIGHT_DRIVE,SLOW+RTRIM,FORWARD);//motor command to the H-bridge, (The motor effected, speed, direction)

				LCDGoToPosition(1,2);// Displays the current state on the second line of the LCD
				sprintf(lcd_str_out,"SLOW_FORWARD");
				LCDSendString(lcd_str_out);
				
				// Allowed state transfer below:

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

				HBridgeCommand(LEFT_DRIVE,FAST+LTRIM,FORWARD);//motor command to the H-bridge, (The motor effected, speed, direction)
				HBridgeCommand(RIGHT_DRIVE,SOFT_TURN+RTRIM,FORWARD);//motor command to the H-bridge, (The motor effected, speed, direction)

				LCDGoToPosition(1,2);// Displays the current state on the second line of the LCD
				sprintf(lcd_str_out,"SOFT_LEFT");
				LCDSendString(lcd_str_out);
				
				// Allowed state transfer below:

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

				HBridgeCommand(LEFT_DRIVE, SOFT_TURN+LTRIM,FORWARD);//motor command to the H-bridge, (The motor effected, speed, direction)
				HBridgeCommand(RIGHT_DRIVE,FAST+RTRIM,FORWARD);//motor command to the H-bridge, (The motor effected, speed, direction)

				LCDGoToPosition(1,2);// Displays the current state on the second line of the LCD
				sprintf(lcd_str_out,"SOFT_RIGHT");
				LCDSendString(lcd_str_out);
				
				// Allowed state transfer below:v

				if(serialInput == 'F'){
					state = FAST_FORWARD;
				}
				else if(serialInput == 'W'){
					state = SLOW_FORWARD;
				}
				else if(serialInput == 'R'){
					state = SOFT_RIGHT;
				}
				else if(serialInput == 'H'){
					state = HARD_RIGHT;
				}
				else if (serialInput == 'S'){
					state = STATE_STOP;
				}

				break;

				case HARD_LEFT:

				HBridgeCommand(LEFT_DRIVE,FAST+LTRIM,FORWARD);//motor command to the H-bridge, (The motor effected, speed, direction)
				HBridgeCommand(RIGHT_DRIVE,HARD_TURN,BACKWARDS);//motor command to the H-bridge, (The motor effected, speed, direction)

				LCDGoToPosition(1,2);// Displays the current state on the second line of the LCD
				sprintf(lcd_str_out,"HARD_LEFT");
				LCDSendString(lcd_str_out);
				
				// Allowed state transfer below:

				if(serialInput == 'F'){
					state = FAST_FORWARD;
				}
				else if(serialInput == 'W'){
					state = SLOW_FORWARD;
				}
				else if(serialInput == 'L'){
					state = SOFT_LEFT;
				}
				else if(serialInput == 'T'){
					state = HARD_LEFT;
				}
				else if (serialInput == 'S'){
					state = STATE_STOP;
				}

				break;

				case HARD_RIGHT:

				HBridgeCommand(LEFT_DRIVE,HARD_TURN,BACKWARDS);//motor command to the H-bridge, (The motor effected, speed, direction)
				HBridgeCommand(RIGHT_DRIVE,FAST+RTRIM,FORWARD);//motor command to the H-bridge, (The motor effected, speed, direction)

				LCDGoToPosition(1,2);// Displays the current state on the second line of the LCD
				sprintf(lcd_str_out,"HARD_RIGHT");
				LCDSendString(lcd_str_out);
				
				// Allowed state transfer below:

				if(serialInput == 'F'){
					state = FAST_FORWARD;
				}
				else if(serialInput == 'W'){
					state = SLOW_FORWARD;
				}
				else if(serialInput == 'R'){
					state = SOFT_RIGHT;
				}
				else if(serialInput == 'H'){
					state = HARD_RIGHT;
				}
				else if (serialInput == 'S'){
					state = STATE_STOP;
				}

				break;

				case STATE_STOP:

				HBridgeCommand(LEFT_DRIVE,STOP,FORWARD);//motor command to the H-bridge, (The motor effected, speed, direction)
				HBridgeCommand(RIGHT_DRIVE,STOP,FORWARD);//motor command to the H-bridge, (The motor effected, speed, direction)

				LCDGoToPosition(1,2);// Displays the current state on the second line of the LCD
				sprintf(lcd_str_out,"STATE_STOP");
				LCDSendString(lcd_str_out);

				// Allowed state transfer below:

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
				
	//This code is printing an array of characters through a UART communication interface.
		for(uint8_t ii =0; ii<20; ii++){
			uart_putchar(serial_char_array_out[ii],&mystdout);
		}
		uart_putchar('\n',&mystdout);
	}
}