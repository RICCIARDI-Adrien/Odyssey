/** @file Main.c
 * Manually control the robot and all of its configuration parameters.
 * @author Adrien RICCIARDI
 * @version 1.1 : 27/09/2014
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Interface.h"
#include "Robot.h"

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Read the battery voltage each second.
 * @param Pointer_Parameters This parameter is unused.
 * @return Nothing.
 */
void *ThreadReadBatteryVoltage(void *Pointer_Parameters)
{
	float Battery_Voltage;
	int Is_Led_Lighted = 1, Percentage;

	while (1)
	{
		// Get voltage
		Battery_Voltage = RobotReadBatteryVoltage();

		// Compute percentage
		Percentage = ((Battery_Voltage - ROBOT_MINIMUM_BATTERY_VOLTAGE) / (ROBOT_MAXIMUM_BATTERY_VOLTAGE - ROBOT_MINIMUM_BATTERY_VOLTAGE)) * 100.f;
		// Adjust values to stay in [0, 100] range
		if (Percentage < 0) Percentage = 0;
		else if (Percentage > 100) Percentage = 100;

		// Display values
		InterfaceDisplayBatteryVoltage(Percentage, Battery_Voltage);

		// Blink LED
		RobotSetLedState(Is_Led_Lighted);
		Is_Led_Lighted = !Is_Led_Lighted;

		usleep(1000000);
	}
}

/** Read the current speeds for the motors and display them. */
static void MainDisplayMotorsSpeeds(void)
{
	int Left_Motor_Forward_Speed, Left_Motor_Backward_Speed, Right_Motor_Forward_Speed, Right_Motor_Backward_Speed;

	// Read all speeds
	Left_Motor_Forward_Speed = RobotReadMotorSpeed(ROBOT_MOTOR_LEFT, ROBOT_MOTOR_DIRECTION_FORWARD);
	Left_Motor_Backward_Speed = RobotReadMotorSpeed(ROBOT_MOTOR_LEFT, ROBOT_MOTOR_DIRECTION_BACKWARD);
	Right_Motor_Forward_Speed = RobotReadMotorSpeed(ROBOT_MOTOR_RIGHT, ROBOT_MOTOR_DIRECTION_FORWARD);
	Right_Motor_Backward_Speed = RobotReadMotorSpeed(ROBOT_MOTOR_RIGHT, ROBOT_MOTOR_DIRECTION_BACKWARD);

	InterfaceDisplaySpeeds(Left_Motor_Forward_Speed, Left_Motor_Backward_Speed, Right_Motor_Forward_Speed, Right_Motor_Backward_Speed);
}

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(void)
{
	pthread_t Thread_ID;

	// Connect to the robot
	if (!RobotInit("/dev/ttyAMA0"))
	{
		puts("Error : can't connect to the robot.");
		return EXIT_FAILURE;
	}

	// Create threads
	if (pthread_create(&Thread_ID, NULL, ThreadReadBatteryVoltage, NULL) != 0)
	{
		puts("Error : can't create battery voltage thread.");
		return EXIT_FAILURE;
	}

	// Initialize interface
	if (!InterfaceInit())
	{
		puts("Error : can't initialize interface. Make sure ncurses library is installed.");
		return EXIT_FAILURE;
	}
	InterfaceDisplayDirection("STOPPED");
	MainDisplayMotorsSpeeds();
	InterfaceDisplayHints();

	while (1)
	{
		switch (getchar())
		{
			case 'z':
				InterfaceDisplayDirection("FORWARD");
				RobotSetMotion(ROBOT_MOTION_FORWARD);
				break;

			case 's':
				InterfaceDisplayDirection("BACKWARD");
				RobotSetMotion(ROBOT_MOTION_BACKWARD);
				break;

			case 'q':
				InterfaceDisplayDirection("LEFT");
				RobotSetMotion(ROBOT_MOTION_FORWARD_TURN_LEFT);
				break;

			case 'd':
				InterfaceDisplayDirection("RIGHT");
				RobotSetMotion(ROBOT_MOTION_FORWARD_TURN_RIGHT);
				break;

			// Stop robot
			case ' ':
				InterfaceDisplayDirection("STOPPED");
				RobotSetMotion(ROBOT_MOTION_STOPPED);
				break;

			// Increase left motor forward speed
			case 'u':
				RobotChangeMotorSpeed(ROBOT_MOTOR_LEFT, ROBOT_MOTOR_DIRECTION_FORWARD, 1);
				MainDisplayMotorsSpeeds();
				break;

			// Decrease left motor forward speed
			case 'j':
				RobotChangeMotorSpeed(ROBOT_MOTOR_LEFT, ROBOT_MOTOR_DIRECTION_FORWARD, 0);
				MainDisplayMotorsSpeeds();
				break;

			// Increase right motor forward speed
			case 'i':
				RobotChangeMotorSpeed(ROBOT_MOTOR_RIGHT, ROBOT_MOTOR_DIRECTION_FORWARD, 1);
				MainDisplayMotorsSpeeds();
				break;

			// Decrease right motor forward speed
			case 'k':
				RobotChangeMotorSpeed(ROBOT_MOTOR_RIGHT, ROBOT_MOTOR_DIRECTION_FORWARD, 0);
				MainDisplayMotorsSpeeds();
				break;

			// Increase left motor backward speed
			case 'o':
				RobotChangeMotorSpeed(ROBOT_MOTOR_LEFT, ROBOT_MOTOR_DIRECTION_BACKWARD, 1);
				MainDisplayMotorsSpeeds();
				break;

			// Decrease left motor backward speed
			case 'l':
				RobotChangeMotorSpeed(ROBOT_MOTOR_LEFT, ROBOT_MOTOR_DIRECTION_BACKWARD, 0);
				MainDisplayMotorsSpeeds();
				break;

			// Increase right motor backward speed
			case 'p':
				RobotChangeMotorSpeed(ROBOT_MOTOR_RIGHT, ROBOT_MOTOR_DIRECTION_BACKWARD, 1);
				MainDisplayMotorsSpeeds();
				break;

			// Decrease right motor backward speed
			case 'm':
				RobotChangeMotorSpeed(ROBOT_MOTOR_RIGHT, ROBOT_MOTOR_DIRECTION_BACKWARD, 0);
				MainDisplayMotorsSpeeds();
				break;

			// Exit
			case 'x':
				RobotSetMotion(ROBOT_MOTION_STOPPED);
				RobotSetLedState(0);
				InterfaceTerminate();
				return EXIT_SUCCESS;
		}
	}
}
