/** @file Main.c
 * Main loop.
 * @author Adrien RICCIARDI
 * @version 1.0 : 15/12/2013
 * @version 1.1 : 01/03/2014, added motor speed calibration feature.
 * @version 1.2 : 02/03/2014, added magic number check to avoid UART glitches.
 */
#include <system.h>
#include "Driver_ADC.h"
#include "Driver_UART.h"
#include "Motor.h"
#include "Robot.h"

//--------------------------------------------------------------------------------------------------
// Private constants
//--------------------------------------------------------------------------------------------------
// Configure PIC16F876 fuses
#pragma DATA _CONFIG, _PWRTE_ON & _BODEN_ON & _WDT_OFF & _LVP_OFF & _CPD_OFF & _DEBUG_OFF & _XT_OSC & _CP_OFF
// Configure clock frequency (Hz)
#pragma CLOCK_FREQ 3686400

//--------------------------------------------------------------------------------------------------
// Private functions
//--------------------------------------------------------------------------------------------------
void interrupt(void)
{
	static unsigned char Is_Magic_Number_Received = 0;
	static unsigned short Battery_Voltage = 0;
	unsigned char Command;
	TMotor Motor;
	TMotorState State;
	TMotorDirection Direction;
	unsigned short Motor_Speed;
	
	// Handle ADC interrupt before the UART one to grant a correct voltage value when entering the UART interrupt
	if (ADCHasInterruptOccured())
	{
		Battery_Voltage = ADCReadLastSample();
		ADCClearInterruptFlag();
	}
	
	// Handle UART receive interrupt
	if (UARTHasInterruptOccured())
	{
		Command = UARTReadByte();
		
		// Wait for a leading magic number
		if (!Is_Magic_Number_Received)
		{
			if (Command == ROBOT_COMMAND_MAGIC_NUMBER) Is_Magic_Number_Received = 1;
			// Re-enable UART interrupt
			UARTClearInterruptFlag();
			return;
		}
		
		// Reached when the magic number was previously received
		Is_Magic_Number_Received = 0; // Reset for next instruction
		
		// Check command opcode
		switch (Command & ROBOT_COMMANDS_CODE_MASK)
		{
			// Set motor state
			case ROBOT_COMMAND_SET_MOTOR_STATE:
				// Extract motor ID
				if (Command & 0x10) Motor = MOTOR_RIGHT;
				else Motor = MOTOR_LEFT;
				
				// Extract requested state
				Command = (Command >> 2) & 0x03;
				if (Command == 1) State = MOTOR_STATE_FORWARD;
				else if (Command == 2) State = MOTOR_STATE_BACKWARD;
				else State = MOTOR_STATE_STOPPED; // So an unknown state will stop the motor
				
				// Apply new state
				MotorSetState(Motor, State);
				break;
				
			// Light or turn off the green LED
			case ROBOT_COMMAND_SET_LED_STATE:
				// Bit 4 tells the led state
				if (Command & 0x10) RobotLedOn();
				else RobotLedOff();
				break;
				
			// Return the last sampled battery voltage value
			case ROBOT_COMMAND_READ_BATTERY_VOLTAGE:
				// Send value in big endian mode
				UARTWriteByte(Battery_Voltage >> 8);
				UARTWriteByte(Battery_Voltage);
				break;
				
			// Increase or decrease a motor speed
			case ROBOT_COMMAND_CHANGE_MOTOR_SPEED:
				// Extract motor ID
				if (Command & 0x10) Motor = MOTOR_RIGHT;
				else Motor = MOTOR_LEFT;
				
				// Extract direction
				if (Command & 0x08) Direction = MOTOR_DIRECTION_BACKWARD;
				else Direction = MOTOR_DIRECTION_FORWARD;
				
				// Extract speed
				Command = (Command & 0x04) >> 2;
				
				MotorChangeSpeed(Motor, Direction, Command);
				break;
				
			// Get a motor current speed
			case ROBOT_COMMAND_READ_MOTOR_SPEED:
				// Extract motor ID
				if (Command & 0x10) Motor = MOTOR_RIGHT;
				else Motor = MOTOR_LEFT;
				
				// Extract direction
				if (Command & 0x08) Direction = MOTOR_DIRECTION_BACKWARD;
				else Direction = MOTOR_DIRECTION_FORWARD;
				
				Motor_Speed = MotorReadSpeed(Motor, Direction);
				
				// Send value in big endian mode
				UARTWriteByte(Motor_Speed >> 8);
				UARTWriteByte(Motor_Speed);
				break;
		}
	
		// Re-enable UART interrupt
		UARTClearInterruptFlag();
	}
}

//--------------------------------------------------------------------------------------------------
// Entry point
//--------------------------------------------------------------------------------------------------
void main(void)
{
	// Initialize robot
	RobotInitialize();
	MotorInitialize();
	ADCInitialize();
	UARTInitialize(UART_BAUD_RATE_115200);
	
	// Stop motors
	MotorSetState(MOTOR_LEFT, MOTOR_STATE_STOPPED);
	MotorSetState(MOTOR_RIGHT, MOTOR_STATE_STOPPED);
		
	// Turn off LED to let master light it when it has finished booting
	RobotLedOff();
	
	// Enable interrupts
	intcon = 0xC0;
	
	// Main loop
	while (1)
	{
		// Read battery voltage each second
		ADCStartSampling();
		delay_s(1);
	}
}