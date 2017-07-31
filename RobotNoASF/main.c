/*
* main.c
*
* Author : et. al
* Created: Unknown
*
* Project Repository: https://github.com/AdamParlane/aut-swarm-robotics
*
* main c file for robot source code for BE (Hons) / BEng Tech Final year industrial project 2017
*
* More Info:
* Atmel SAM 4N Processor Datasheet:http://www.atmel.com/Images/Atmel-11158-32-bit%20Cortex-M4-Microcontroller-SAM4N16-SAM4N8_Datasheet.pdf
* Relevant reference materials or datasheets if applicable
*
* Functions:
* int main(void)
* void setup(void)
*/

///////////////Includes/////////////////////////////////////////////////////////////////////////////
#include "robot_defines.h"

///////////////Defines//////////////////////////////////////////////////////////////////////////////
#define		batteryLow	1


///////////////Global variables/////////////////////////////////////////////////////////////////////
uint8_t SBtest, SBtest1;
uint16_t DBtest, DBtest1, DBtest2;

///////////////Functions////////////////////////////////////////////////////////////////////////////
/*
* Function:
* void setup(void)
*
* The initialisation routine for all hardware in the robot.
*
* Inputs:
* none
*
* Returns:
* none
*
*/
void setup(void);

/*
* Function:
* int main(void)
*
* Overview of robot code execution
*
* Inputs:
* none
*
* Returns:
* N/A
*
* Implementation:
* [[[[[WIP]]]]]
*
* Improvements:
* Yes.
*
*/
int main(void)
{
	setup();
	uint8_t testMode;
	//Comms
	struct frame_info frame;
	struct message_info message;
	//Optical
	struct Position robotPosition;
	robotPosition.x = 0;
	robotPosition.y = 0;
	struct transmitDataStructure transmitMessage;
	robotState = IDLE;
	while(1)
	{
		switch (robotState)
		{
			case TEST:
				if(newDataFlag || streamIntervalFlag)//get the new test data
				{
					testMode = testManager(message, &transmitMessage);//get the new test data
				}
				if(testMode == STOP_STREAMING)
					robotState = IDLE;
				else if(testMode == SINGLE_SAMPLE)
				{
					robotState = IDLE;
					SendXbeeAPITransmitRequest(BROADCAST_64,UNKNOWN_16, transmitMessage.Data, transmitMessage.DataSize);  //Send the Message
				}
				else if(streamIntervalFlag && testMode == STREAM_DATA)
				{
					streamIntervalFlag = 0;
					SendXbeeAPITransmitRequest(BROADCAST_64,UNKNOWN_16, transmitMessage.Data, transmitMessage.DataSize);  //Send the Message
				}
			break;
			
			case TEST_ALL:
			break;
			
			case MANUAL:
			moveRobot(90, 50);
			delay_ms(10000);
			stopRobot();
			robotState = IDLE;
			break;
			
			case DOCKING:
			//if battery low or manual command set
				dockRobot();
			break;
			
			case FORMATION:
			//placeholder
			break;
			
			case IDLE:
			//idle
			PWMSpeedTest();
			break;
		}
		
		if(FrameBufferInfoGetFull(&frame) == 0)	//Check for a received XBee Message
		{
			InterpretXbeeAPIFrame(frame); //Interpret the received XBee Message

			if(MessageBufferInfoGetFull(&message) == 0) //Check for a message from the swarm
			{
				InterpretSwarmMessage(message);	//Interpret the message
			}
		}
	}
}

/*
* Function:
* void setup(void)
*
* The initialisation routine for all hardware in the robot.
*
* Inputs:
* none
*
* Returns:
* none
*
* Implementation:
* Contains functions which systematically set up each peripheral and hardware device connected to
* the robot's micro controller. Click on a function and press 'Alt + G' to open the file where it
* is kept (if using Atmel Studio)
*
* Improvements:
* Maybe
*
*/
void setup(void)
{
	//////////////Setup routine has been modified to just test IMU in this branch./////////////////
	REG_WDT_MR = WDT_MR_WDDIS; 			//Disable system watchdog timer.

	masterClockInit();					//Initialise the master clock to 100MHz
	pioInit();							//Initialise the PIO controllers
	adcSingleConvInit();				//Initialise ADC for single conversion mode
	ledInit();							//Initialise the LEDs on the mid board
	motor_init();						//Initialise the motor driver chips
	SPI_Init();							//Initialise SPI for talking with optical sensor
	twi0Init();							//Initialise TWI0 interface
	twi2Init();							//Initialise TWI2 interface
	//lightSensInit(MUX_LIGHTSENS_R);		//Initialise Right Light/Colour sensor
	//lightSensInit(MUX_LIGHTSENS_L);		//Initialise Left Light/Colour sensor
	//proxSensInit(MUX_PROXSENS_A);		//Initialise proximity sensor on panel A
	//proxSensInit(MUX_PROXSENS_B);		//Initialise proximity sensor on panel B
	//proxSensInit(MUX_PROXSENS_C);		//Initialise proximity sensor on panel C
	//proxSensInit(MUX_PROXSENS_D);		//Initialise proximity sensor on panel D
	//proxSensInit(MUX_PROXSENS_E);		//Initialise proximity sensor on panel E
	//proxSensInit(MUX_PROXSENS_F);		//Initialise proximity sensor on panel F
	//fcInit();							//Initialise the fast charge chip
	//CommunicationSetup();				//Initialise communication system
	//imuInit();							//Initialise IMU
	int result = imuCommTest();
	mouseInit();						//May require further testing - Adam
	return;
}

