/*
* motor_driver.c
*
* Author : Adam Parlane and Matthew Witt
* Created: 13/05/2017 4:16:25 PM
*
* Project Repository: https://github.com/AdamParlane/aut-swarm-robotics
*
* Provides functions for controlling the motors and moving the robot.
*
* More Info:
* Atmel SAM 4N Processor Datasheet:http://www.atmel.com/Images/Atmel-11158-32-bit%20Cortex-M4-Microcontroller-SAM4N16-SAM4N8_Datasheet.pdf
* BD6211 Motor driver Datasheet:http://rohmfs.rohm.com/en/products/databook/datasheet/ic/motor/dc/bd621x-e.pdf
*
* The 3 motors are driven by 1 H-Bridge each with a differential output on pins OUT1 & OUT2
* This controls the speed and direction of the motor
* The H-Bridges are controlled by the micro using 3 Pins per H-Bridge and PWM
* Each H-Bridge uses FIN_x, RIN_x & VREF_x (where x is the motor number)
* FIN and RIN control the motor direction, 1 and only 1 must be high to enable the desired direction
* VREF uses PWM with duty cycle 0-100(%) to set the speed of the motor
*
* Functions:
* void motor_init(void);
* void moveRobot(float direction, unsigned char speed);
* void wiggleForward(uint8_t forwardSpeed, uint8_t lateralSpeed, uint8_t direction)
* void stopRobot(void);
* void rotateRobot(char direction, unsigned char speed);
* void dockRobot(void);
* void setTestMotors(uint8_t motorData[]);
* 
*/
 
///////////////Includes/////////////////////////////////////////////////////////////////////////////
#include "motor_driver.h"

///////////////Functions////////////////////////////////////////////////////////////////////////////
/*
* Function:
* void motor_init(void)
*
* Initializes micro controller's PWM feature and PIO on the pins connected to the motor drivers.
*
* Inputs:
* none
*
* Returns:
* none
*
* Implementation:
* Firstly for ROBOT V1 PB12 is used as FIN_1, this is the erase pin
* so the erase functionality is disabled to allow PIO access of PB12
* The PMC is then given master clock access to PWM (PCER0 PID31)
* Motor 1 uses channel 3, motor 2 on channel 2 and motor 3 on channel 1
* In the PWM Channel Mode Register (PWM_CMRx) the following are set
* For each channel the prescale is set to CLK/1
* The PWM output starts at high level (this shouldnt make a difference in the program, high or low)
* The output is left aligned (again this could go either way as long as its consistent)
* Most importantly we want to update the DUTY CYCLE not the period in order to change the motors'
* speeds easily
* The starting duty cycle is set to 0 in PWM_CDTYx as the motors are to initialize as off
* The PWM counter is set to 100, this allows the duty cycle to simply be a percentage
* Using the PIOx_PDR control of VREFx is given to the peripheral, 
* the next line states that peripheral is PWM peripheral B (same for all channels)
*
* Now that PWM is setup PIO is used to PIO control of FIN_x and RIN_x 
* Then the Output Enable Register (OER) is used to enable FIN_x and RIN_x as outputs
* 
* Now that everything is setup the PWM channels are enabled with the PWM_ENA registers

* Improvements:
* TODO: convert code to be using the SAM4N macros
*
*/
void motor_init(void)
{
#if defined ROBOT_TARGET_V1
	REG_CCFG_SYSIO |= CCFG_SYSIO_SYSIO12; //disable erase pin to give access to PB12 via PIO
#endif
	REG_PMC_PCER0 |= PMC_PCER0_PID31;	//Enable clock access for PWM
	
	//****Channel 3 (Motor 1)****//
	REG_PWM_CMR3 |= (0x4<<0);		//Channel pre scale CLK by 16 = 24.4KHz
	REG_PWM_CMR3 |= (1<<9);			//output starts at high level
	REG_PWM_CMR3 &= ~(1<<8);		//Left aligned output
	REG_PWM_CMR3 &= ~(1<<10);		//Update Duty cycle (NOT period)
	REG_PWM_CDTY3 = 0;				//PWM Duty cycle (default = 0)
	REG_PWM_CPRD3 = 100;			//PWM Counter 0 - 100 (makes duty cycle %)
		
	REG_PIOC_PDR |= (1<<21);		//Enable peripheral control of PC21
	REG_PIOC_ABCDSR = (1<<21);		//Assign PC21 to PWM Peripheral B		
#if defined ROBOT_TARGET_V1
	REG_PIOB_PER |= (1<<12);		//Enable PIO control of PB12
	REG_PIOB_OER |= (1<<12);		//Set PB12 as output
#endif
#if defined ROBOT_TARGET_V2
	REG_PIOC_PER |= (1<<23);		//Enable PIO control of PB12
	REG_PIOC_OER |= (1<<23);		//Set PB12 as output
#endif
	FIN_1_L;		
	REG_PIOC_PER |= (1<<22);		//Enable PIO control of PC22
	REG_PIOC_OER |= (1<<22);		//Set PC22 as output
	RIN_1_L;
								
	//****Channel 2 (Motor 2)****//
	REG_PWM_CMR2 |= (0x4<<0);		//Channel pre scale CLK by 16 = 24.4KHz
	REG_PWM_CMR2 |= (1<<9);			//output starts at low level
	REG_PWM_CMR2 &= ~(1<<8);		//Left aligned output
	REG_PWM_CMR2 &= ~(1<<10);		//Update Duty cycle (NOT period)
	REG_PWM_CDTY2 = 0;				//PWM Duty cycle (default = 0)
	REG_PWM_CPRD2 = 100;			//PWM Counter 0 - 100 (makes duty cycle %)
		
	REG_PIOC_PDR |= (1<<20);		//Enable peripheral control of PC20
	REG_PIOC_ABCDSR = (1<<20);		//Assign VREF_1 to PWM Peripheral B			
	REG_PIOC_PER |= (1<<19);		//Enable PIO control of PC19
	REG_PIOC_OER |= (1<<19);		//Set PC19 as output
	RIN_2_L;		
	REG_PIOA_PER |= (1<<31);		//Enable PIO control of PA31
	REG_PIOA_OER |= (1<<31);		//Set PA31 as output
	FIN_2_L;		

	//****Channel 1 (Motor 3)****//
	REG_PWM_CMR1 |= (0x4<<0);		//Channel pre scale CLK by 16 = 24.4KHz
	REG_PWM_CMR1 |= (1<<9);			//output starts at low level
	REG_PWM_CMR1 &= ~(1<<8);		//Left aligned output
	REG_PWM_CMR1 &= ~(1<<10);		//Update Duty cycle (NOT period)
	REG_PWM_CDTY1 = 0;				//PWM Duty cycle (default = 0)
	REG_PWM_CPRD1 = 100;			//PWM Counter 0 - 100 (makes duty cycle %)
		
	REG_PIOC_PDR |= (1<<9);			//Enable peripheral control of PC9
	REG_PIOC_ABCDSR = (1<<9);		//Assign PC9 to PWM Peripheral B		
	REG_PIOA_PER |= (1<<29);		//Enable PIO control of PA30
	REG_PIOA_OER |= (1<<29);		//Set PA30 as output
	RIN_3_L;		
#if defined ROBOT_TARGET_V1
	REG_PIOA_PER |= (1<<30);		//Enable PIO control of PA29
	REG_PIOA_OER |= (1<<30);		//Set PA29 as output
#endif
#if defined ROBOT_TARGET_V2
	REG_PIOC_PER |= (1<<10);		//Enable PIO control of PC10
	REG_PIOC_OER |= (1<<10);		//Set PC10 as output
#endif
	FIN_3_L;
	
	//****Enable PWM Channels as last step of setup****//	
	REG_PWM_ENA |= PWM_ENA_CHID1;	//Enable PWM on channel 1
	REG_PWM_ENA |= PWM_ENA_CHID2;	//Enable PWM on channel 2
	REG_PWM_ENA |= PWM_ENA_CHID3;	//Enable PWM on channel 3
}

/*
* Function:
* void moveRobot(float direction, unsigned char speed)
*
* Will start the robot moving in the desired heading at the desired speed
*
* Inputs:
* float direction:
*	heading in degrees in which the robot should move. Irrelevant if speed = 0
* unsigned char speed
*	Speed at which robot should move. Is a percentage of maximum speed (0-100)
*
* Returns:
*	none
*
* Implementation:
* The direction is kept in range of +/-180 by removing whole chunks of 360 deg to it
* The speed is also limited at 100, so any speed over 100 is set to 100
* The direction is then converted to radians to allow smooth working with cos()
* Each motors' speed is calculated using the FORWARD DRIVE DIRECTION of the motor, with respect to
* the front face of the robot
* These are:	Motor 1		270 deg
*				Motor 2		30  deg
*				Motor 3		150 deg
* The robot speed is then cos(motor direction - desired robot direction) (performed in radians)
* This will produce a ratio up to 1 of the full speed in order to achieve the correct direction
* This is also multiplied by the desired speed to achieve an overall robot speed
* The long part with if statments simply turns on the correct FIN and RIN pins to achieve the 
* desired MOTOR direction, this is based off the speed calculation which could return negative
* Finally the ABSOLUTE value of the motorxspeed is written to the PWM_CUPD register to update
* the duty cycle.
*
* Improvements:
* None as of 26/7/17
*
*/
void moveRobot(uint16_t direction, unsigned char speed)
{
	uint16_t motor1Speed, motor2Speed, motor3Speed;
	float directionRad;
	//keep direction in range +/-180degrees
	while(direction > 180) 
		direction -= 360;
	while(direction < -180)
		direction += 360;
	//stop speed from being over max in case of user input error
	if(speed > 100)
		speed = 100;
	directionRad = (direction * M_PI) / 180; //convert desired direction to radians
	motor1Speed = speed * cos ((270 * M_PI) / 180 - directionRad );//radians
	motor2Speed = speed * cos ((30  * M_PI) / 180 - directionRad );
	motor3Speed = speed * cos ((150 * M_PI) / 180 - directionRad );
	//motor 2 & 3 is wired backwards on test robot so forward and back is flipped
	if(motor1Speed > 0)
	{
		//Forward
		RIN_1_L;
		FIN_1_H;		
	}
	else if (motor1Speed == 0)
	{
		//Motor off (coast not brake)
		RIN_1_L;
		FIN_1_L;
	}
	else
	{
		//Reverse
		motor1Speed = motor1Speed * (-1); 
		RIN_1_H;
		FIN_1_L;
	}

	if(motor2Speed > 0)
	{
		//Forward
		RIN_2_L;
		FIN_2_H;
	}
	else if (motor2Speed == 0)
	{
		//Motor off (coast not brake)
		RIN_2_L;
		FIN_2_L;
	}
	else
	{
		//Reverse
		motor2Speed = motor2Speed * (-1); 
		RIN_2_H;
		FIN_2_L;
	}
	
	if(motor3Speed > 0)
	{
		//Forward
		RIN_3_L;
		FIN_3_H;			
	}
	else if (motor3Speed == 0)
	{
		//Motor off (coast not brake)
		RIN_3_L;
		FIN_3_L;
	}
	else
	{
		//Reverse
		motor3Speed = motor3Speed * (-1); 
		RIN_3_H;
		FIN_3_L;
	}
	REG_PWM_CUPD1 = (motor3Speed); //Update duty cycle as per calculations
	REG_PWM_CUPD2 = (motor2Speed); //Update duty cycle as per calculations
	REG_PWM_CUPD3 = (motor1Speed); //Update duty cycle as per calculations
}

/*
* Function:
* void rotateRobot(char direction, unsigned char speed)
*
* Will rotate the robot on the spot in the given direcion and relative speed.
*
* Inputs:
* char direction
*	The direction the robot should rotate (CW or CCW)
* unsigned char speed
*	Speed at which robot should move. Is a percentage of maximum speed (0-100)
*
* Returns:
* none
*
* Implementation:
* To rotate Clockwise (CW) all FIN should be LOW and all RIN HIGH (all motors spin in reverse)
* To rotate Counterclockwise (CCW) all FIN should be HIGH and all RIN LOW (all motors forward)
* This is set with 2 simple if statements
* All motors should spin at the same speed which is simply the input arguement speed
* This is used to update the duty cycle using PWM_CUPD
*
* Improvements:
* None as of 26/7/17
*
*/
void rotateRobot(char direction, unsigned char speed)
{
	if(direction == CW)			//enable all motors to spin the robot clockwise
	{
		RIN_1_H;
		FIN_1_L;
		RIN_2_H;
		FIN_2_L;
		RIN_3_H;
		FIN_3_L;
	}
	else if(direction == CCW)	//enable all motors to spin the robot counter-clockwise
	{
		RIN_1_L;
		FIN_1_H;
		RIN_2_L;
		FIN_2_H;
		RIN_3_L;
		FIN_3_H;
	}
	//Update all duty cycles to match the desired rotation speed
	REG_PWM_CUPD1 = speed;
	REG_PWM_CUPD2 = speed;
	REG_PWM_CUPD3 = speed;	
}

/*
* Function:
* void wiggleForward(uint8_t forwardSpeed, uint8_t lateralSpeed, uint8_t direction)
*
* Will move robot forward at desired speed forward, but will also allow front motor to be turned on
* so that direction of travel will become an arc to the left or right. Allows for line following and
* docking.
*
* Inputs:
* uint8_t forwardSpeed:
*   Percentage of full speed that the robot will move forward at (0-100)
*
* uint8_t lateralSpeed:
*   Percentage of full speed that the lateral wheel will be allowed to spin at to steer robot left
*   or right. (0-100)
*
* uint8_t direction:
*   Direction that robot will rotate towards on its arc (CW and CCW)
*
* Returns:
* none
*
* Implementation:
* TODO: Better documentation, Tidier layout.
* - Needs more work and fine tuning in general.
* - Turn on front two motors and set their speed
* - Turn on rear (lateral) motor and set its speed and direction.
*
* Improvements:
* May add ability to move backwards in this manner too.
*
*/
void wiggleForward(uint8_t forwardSpeed, uint8_t lateralSpeed, uint8_t direction)
{
	//Make sure parameters are within range.
	if (forwardSpeed > 100)
		forwardSpeed = 0;
	if (lateralSpeed > 100)
		lateralSpeed = 0;
		
	//lateralSpeed /= 2;						//Scale down lateral speed value so it doesn't overwhelm
											//forward speed
	switch(direction)
	{
		case CW:
			RIN_2_H;
			FIN_2_L;
		break;
		
		case CCW:
			RIN_2_L;
			FIN_2_H;
		break;
	}

	//Set front forward motion direction
	RIN_1_H;
	FIN_1_L;
	RIN_3_L;
	FIN_3_H;
	
	if (forwardSpeed+lateralSpeed > 100)		//If forwardSpeed+lateralSpeed will go over 100
												//Then scale down speed values to fit
	{
		switch(direction)
		{
			case CW:
				REG_PWM_CUPD3 = 100;							//Left Front
				REG_PWM_CUPD1 = 100 - lateralSpeed*2;			//Right front
			break;
		
			case CCW:
				REG_PWM_CUPD1 = 100;							//Right front
				REG_PWM_CUPD3 = 100 - lateralSpeed*2;			//Left front
			break;
		}		
	} else {
		switch(direction)
		{
			case CW:
				REG_PWM_CUPD1 = forwardSpeed-lateralSpeed;		//Right front
				REG_PWM_CUPD3 = forwardSpeed+lateralSpeed;		//Left front
			break;
			
			case CCW:
				REG_PWM_CUPD3 = forwardSpeed-lateralSpeed;		//Left front
				REG_PWM_CUPD1 = forwardSpeed+lateralSpeed;		//Right front
			break;
		}
	}
	
	REG_PWM_CUPD2 = lateralSpeed;		//Rear motor speed (Rear motor)
}

/*

* Function:
* void stopRobot(void)
*
* Stop all motors
*
* Inputs:
* none
*
* Returns:
* none
*
* Implementation:
* Sets all direction pins on the motor drivers low.
*
*/
void stopRobot(void)
{
	//Stops the robot from moving
	FIN_1_L;
	RIN_1_L;
	FIN_2_L;
	RIN_2_L;
	FIN_3_L;
	RIN_3_L;
	REG_PWM_CUPD1 = 0;
	REG_PWM_CUPD2 = 0;
	REG_PWM_CUPD3 = 0;
}

/*
* Function:
* void setTestMotors(uint8_t motorData[])
*
* [brief purpose of function]
*
* Inputs:
* uint8_t motorData[]
*   two element 8bit array that contains a data packet from PC GUI relevant to the motor test
*   routine. (ie speed, direction and what motor to run)
*
* Returns:
* none
*
* Implementation:
* For the robot test motorData[0] describes which motor to test
* motorData[1] describes both the speed (bits 0-6) and direction (bit 7) of the motor
* The speed must be 0-100
* The direction can be set meaning forward or cleared meaning reverse
* Each if statement is checking which motor and which direction
* Once the correct if statement is entered it will set the correct motor and direction
* The speed will be used to update the duty cycle 
* with bit masking to ensure only the first 7 bits are used
*
* Improvements:
* Maybe some of the PWM registers could be given more read-friendly names?
*
*/
void setTestMotors(uint8_t motorData[])
{
	if(motorData[0] == MOTOR_1 && (motorData[1] & 0x80))//check if bit 7 is set meaning forward
	{
		FIN_1_H;
		RIN_1_L;
		REG_PWM_CUPD1 = (motorData[1] & 0x7F);
	}
	else if(motorData[0] == MOTOR_1 && ~(motorData[1] & 0x80))
	{
		FIN_1_L;
		RIN_1_H;
		REG_PWM_CUPD1 = (motorData[1] & 0x7F);
	}
	else if(motorData[0] == MOTOR_2 && (motorData[1] & 0x80))//check if bit 7 is set meaning forward
	{
		FIN_2_H;
		RIN_2_L;
		REG_PWM_CUPD2 = (motorData[1] & 0x7F);
	}
	else if(motorData[0] == MOTOR_2 && ~(motorData[1] & 0x80))
	{
		FIN_2_L;
		RIN_2_H;
		REG_PWM_CUPD2 = (motorData[1] & 0x7F);
	}
	else if(motorData[0] == MOTOR_3 && (motorData[1] & 0x80))//check if bit 7 is set meaning forward
	{
		FIN_3_H;
		RIN_3_L;
		REG_PWM_CUPD3 = (motorData[1] & 0x7F);
	}
	else if(motorData[0] == MOTOR_3 && ~(motorData[1] & 0x80))
	{
		FIN_3_L;
		RIN_3_H;
		REG_PWM_CUPD3 = (motorData[1] & 0x7F);
	}
}

/*
*
* Function:
* void PWMSpeedTest(void)
*
* Runs motor 2 at 10% duty cycle steps for 5 seconds each
*
* Inputs:
* none
*
* Returns:
* none
*
* Implementation:
* Powers motor 2 0-100% duty cycle in 5 second 10% steps
* Purpose is to test the PWM curve on each robot
*
*/
void PWMSpeedTest(void)
{
	//Stops the robot from moving
	FIN_2_H;
	RIN_2_L;
	REG_PWM_CUPD2 = 0;
	delay_ms(5000);
	REG_PWM_CUPD2 = 0;
	delay_ms(5000);
	REG_PWM_CUPD2 = 10;
	delay_ms(5000);
	REG_PWM_CUPD2 = 0;
	delay_ms(5000);
	REG_PWM_CUPD2 = 20;
	delay_ms(5000);
	REG_PWM_CUPD2 = 0;
	delay_ms(5000);
	REG_PWM_CUPD2 = 30;
	delay_ms(5000);
	REG_PWM_CUPD2 = 0;
	delay_ms(5000);
	REG_PWM_CUPD2 = 40;
	delay_ms(5000);
	REG_PWM_CUPD2 = 0;
	delay_ms(5000);
	REG_PWM_CUPD2 = 50;
	delay_ms(5000);
	REG_PWM_CUPD2 = 0;
	delay_ms(5000);
	REG_PWM_CUPD2 = 60;
	delay_ms(5000);
	REG_PWM_CUPD2 = 0;
	delay_ms(5000);
	REG_PWM_CUPD2 = 70;
	delay_ms(5000);
	REG_PWM_CUPD2 = 0;
	delay_ms(5000);
	REG_PWM_CUPD2 = 80;
	delay_ms(5000);
	REG_PWM_CUPD2 = 0;
	delay_ms(5000);
	REG_PWM_CUPD2 = 90;
	delay_ms(5000);
	REG_PWM_CUPD2 = 0;
	delay_ms(5000);
	REG_PWM_CUPD2 = 100;
	delay_ms(5000);
	stopRobot();
}

