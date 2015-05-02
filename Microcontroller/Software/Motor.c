/** @file Motor.c
 * @see Motor.h for description.
 * @author Adrien RICCIARDI
 */
#include <system.h>
#include "Driver_EEPROM.h"
#include "Motor.h"

//--------------------------------------------------------------------------------------------------
// Private constants and macros
//--------------------------------------------------------------------------------------------------
// PWM pins
#define MOTOR_LEFT_PIN 2
#define MOTOR_RIGHT_PIN 1

// Duty cycle values to make each motor rotate forward and backward
#define MOTOR_LEFT_FORWARD_DEFAULT_DUTY_CYCLE 48140
#define MOTOR_LEFT_BACKWARD_DEFAULT_DUTY_CYCLE 48690
#define MOTOR_RIGHT_FORWARD_DEFAULT_DUTY_CYCLE 48930
#define MOTOR_RIGHT_BACKWARD_DEFAULT_DUTY_CYCLE 47990

// EEPROM addresses of motor speed values
#define MOTOR_LEFT_FORWARD_DUTY_CYCLE_EEPROM_ADDRESS 0
#define MOTOR_LEFT_BACKWARD_DUTY_CYCLE_EEPROM_ADDRESS 2
#define MOTOR_RIGHT_FORWARD_DUTY_CYCLE_EEPROM_ADDRESS 4
#define MOTOR_RIGHT_BACKWARD_DUTY_CYCLE_EEPROM_ADDRESS 6

// PWM hardware limits
/** The 0% duty cycle value. */
#define MOTOR_MINIMUM_DUTY_CYCLE_VALUE 18432
/** The 100% duty cycle value. */
#define MOTOR_MAXIMUM_DUTY_CYCLE_VALUE 65535
/** Make the timer 1 overflows each 50Hz. */
#define MOTOR_PWM_TIMER_RELOAD_VALUE (65535 - MOTOR_MINIMUM_DUTY_CYCLE_VALUE)
/** Enable the timer 1. */
#define MOTOR_PWM_TIMER_START() t1con.TMR1ON = 1
/** Disable the timer 1. */
#define MOTOR_PWM_TIMER_STOP() t1con.TMR1ON = 0

/** How many motors to handle. */
#define MOTORS_COUNT 2

//--------------------------------------------------------------------------------------------------
// Private types
//--------------------------------------------------------------------------------------------------
/** A motor required parameters. */
typedef struct
{
	unsigned short Forward_Duty_Cycle; //! The duty cycle value to make the motor go forward.
	unsigned short Backward_Duty_Cycle; //! The duty cycle value to make the motor go backward.
	unsigned short Current_Duty_Cycle; //! The duty cyle currently used and to apply at the beginning of each PWM cycle.
	TMotorState Current_State; //! The motor current state.
} TMotorParameters;

//--------------------------------------------------------------------------------------------------
// Private variables
//--------------------------------------------------------------------------------------------------
/** All available motors. Do not recurse on this array or the compiler will use a software emulated multiplication to compute the addresses. */
static TMotorParameters Motor_Parameters[MOTORS_COUNT];

//--------------------------------------------------------------------------------------------------
// Public functions
//--------------------------------------------------------------------------------------------------
void MotorInitialize(void)
{
	unsigned char i;
	
	// Configure PWM pins as outputs
	trisc.MOTOR_LEFT_PIN = 0;
	trisc.MOTOR_RIGHT_PIN = 0;
	
	// Stop motors
	portc.MOTOR_LEFT_PIN = 0;
	portc.MOTOR_RIGHT_PIN = 0;
	Motor_Parameters[MOTOR_LEFT].Current_Duty_Cycle = 0;
	Motor_Parameters[MOTOR_RIGHT].Current_Duty_Cycle = 0;
	Motor_Parameters[MOTOR_LEFT].Current_State = MOTOR_STATE_STOPPED;
	Motor_Parameters[MOTOR_RIGHT].Current_State = MOTOR_STATE_STOPPED;
	
	// Check if the EEPROM contains the duty cycle values
	Motor_Parameters[MOTOR_LEFT].Forward_Duty_Cycle = EEPROMReadWord(MOTOR_LEFT_FORWARD_DUTY_CYCLE_EEPROM_ADDRESS);
	
	// Is the EEPROM memory empty ?
	if (Motor_Parameters[MOTOR_LEFT].Forward_Duty_Cycle == 0xFFFF)
	{
		// Write default duty cycles to EEPROM
		EEPROMWriteWord(MOTOR_LEFT_FORWARD_DUTY_CYCLE_EEPROM_ADDRESS, MOTOR_LEFT_FORWARD_DEFAULT_DUTY_CYCLE);
		EEPROMWriteWord(MOTOR_LEFT_BACKWARD_DUTY_CYCLE_EEPROM_ADDRESS, MOTOR_LEFT_BACKWARD_DEFAULT_DUTY_CYCLE);
		EEPROMWriteWord(MOTOR_RIGHT_FORWARD_DUTY_CYCLE_EEPROM_ADDRESS, MOTOR_RIGHT_FORWARD_DEFAULT_DUTY_CYCLE);
		EEPROMWriteWord(MOTOR_RIGHT_BACKWARD_DUTY_CYCLE_EEPROM_ADDRESS, MOTOR_RIGHT_BACKWARD_DEFAULT_DUTY_CYCLE);
	
		// Use default values
		Motor_Parameters[MOTOR_LEFT].Forward_Duty_Cycle = MOTOR_LEFT_FORWARD_DEFAULT_DUTY_CYCLE;
		Motor_Parameters[MOTOR_LEFT].Backward_Duty_Cycle = MOTOR_LEFT_BACKWARD_DEFAULT_DUTY_CYCLE;
		Motor_Parameters[MOTOR_RIGHT].Forward_Duty_Cycle = MOTOR_RIGHT_FORWARD_DEFAULT_DUTY_CYCLE;
		Motor_Parameters[MOTOR_RIGHT].Backward_Duty_Cycle = MOTOR_RIGHT_BACKWARD_DEFAULT_DUTY_CYCLE;
	}
	else
	{
		// Load remaining values from EEPROM
		Motor_Parameters[MOTOR_LEFT].Backward_Duty_Cycle = EEPROMReadWord(MOTOR_LEFT_BACKWARD_DUTY_CYCLE_EEPROM_ADDRESS);
		Motor_Parameters[MOTOR_RIGHT].Forward_Duty_Cycle = EEPROMReadWord(MOTOR_RIGHT_FORWARD_DUTY_CYCLE_EEPROM_ADDRESS);
		Motor_Parameters[MOTOR_RIGHT].Backward_Duty_Cycle = EEPROMReadWord(MOTOR_RIGHT_BACKWARD_DUTY_CYCLE_EEPROM_ADDRESS);
	}
	
	// Configure the CCP modules in Compare mode
	ccp1con = 0x0A;
	ccp2con = 0x0A;
	
	// Configure the timer 1 to generate a 50Hz frequency
	tmr1h = MOTOR_PWM_TIMER_RELOAD_VALUE >> 8;
	tmr1l = (unsigned char) MOTOR_PWM_TIMER_RELOAD_VALUE;
	pie1.TMR1IE = 1; // Enable the timer 1 interrupt
	t1con = 0x01; // Start the timer with no prescaler and the internal clock as clock source
}

void MotorInterruptHandler(void)
{
	MOTOR_PWM_TIMER_STOP();

	// Handle timer 1 overflow : the PWM period is over
	if (pir1.TMR1IF)
	{
		// Update the left motor duty cycle
		// Schedule an interrupt if the left motor duty cycle is in range ]0;100%[
		if ((Motor_Parameters[MOTOR_LEFT].Current_Duty_Cycle > 0) && (Motor_Parameters[MOTOR_LEFT].Current_Duty_Cycle < MOTOR_MAXIMUM_DUTY_CYCLE_VALUE))
		{
			// Update the left motor PWM duty cycle
			ccpr1h = Motor_Parameters[MOTOR_LEFT].Current_Duty_Cycle >> 8;
			ccpr1l = (unsigned char) Motor_Parameters[MOTOR_LEFT].Current_Duty_Cycle;
			pie1.CCP1IE = 1;
		}
		else pie1.CCP1IE = 0;
		// Set the pin state
		if (Motor_Parameters[MOTOR_LEFT].Current_Duty_Cycle == 0) portc.MOTOR_LEFT_PIN = 0;
		else portc.MOTOR_LEFT_PIN = 1;
		
		// Update the right motor duty cycle
		// Schedule an interrupt if the right motor duty cycle is in range ]0;100%[
		if ((Motor_Parameters[MOTOR_RIGHT].Current_Duty_Cycle > 0) && (Motor_Parameters[MOTOR_RIGHT].Current_Duty_Cycle < MOTOR_MAXIMUM_DUTY_CYCLE_VALUE))
		{
			// Update the right motor PWM duty cycle
			ccpr2h = Motor_Parameters[MOTOR_RIGHT].Current_Duty_Cycle >> 8;
			ccpr2l = (unsigned char) Motor_Parameters[MOTOR_RIGHT].Current_Duty_Cycle;
			pie2.CCP2IE = 1;
		}
		else pie2.CCP2IE = 0;
		// Set the pin state
		if (Motor_Parameters[MOTOR_RIGHT].Current_Duty_Cycle == 0) portc.MOTOR_RIGHT_PIN = 0;
		else portc.MOTOR_RIGHT_PIN = 1;
	
		// Reload the timer
		tmr1h = MOTOR_PWM_TIMER_RELOAD_VALUE >> 8;
		tmr1l = (unsigned char) MOTOR_PWM_TIMER_RELOAD_VALUE;
		
		// Reset the interrupt flag
		pir1.TMR1IF = 0;
	}
	
	// Handle the CCP1 interrupt : the left motor pin must change state
	if ((pie1.CCP1IE) && (pir1.CCP1IF))
	{
		portc.MOTOR_LEFT_PIN = 0;
		
		// Reset interrupt flag
		pir1.CCP1IF = 0;
	}
	
	// Handle the CCP2 interrupt : the right motor pin must change state
	if ((pie2.CCP2IE) && (pir2.CCP2IF))
	{
		portc.MOTOR_RIGHT_PIN = 0;
		
		// Reset interrupt flag
		pir2.CCP2IF = 0;
	}
	
	MOTOR_PWM_TIMER_START();
}

// This function is unrolled by hand to prevent the compiler from using software multiplications in addresses calculation
void MotorSetState(TMotor Motor, TMotorState State)
{
	MOTOR_PWM_TIMER_STOP();
	
	// Choose the right duty cycle according to selected state
	switch (State)
	{
		case MOTOR_STATE_STOPPED:
			if (Motor == MOTOR_LEFT) Motor_Parameters[MOTOR_LEFT].Current_Duty_Cycle = 0;
			else Motor_Parameters[MOTOR_RIGHT].Current_Duty_Cycle = 0;
			break;
			
		case MOTOR_STATE_FORWARD:
			if (Motor == MOTOR_LEFT) Motor_Parameters[MOTOR_LEFT].Current_Duty_Cycle = Motor_Parameters[MOTOR_LEFT].Forward_Duty_Cycle;
			else Motor_Parameters[MOTOR_RIGHT].Current_Duty_Cycle = Motor_Parameters[MOTOR_RIGHT].Forward_Duty_Cycle;
			break;
			
		case MOTOR_STATE_BACKWARD:
			if (Motor == MOTOR_LEFT) Motor_Parameters[MOTOR_LEFT].Current_Duty_Cycle = Motor_Parameters[MOTOR_LEFT].Backward_Duty_Cycle;
			else Motor_Parameters[MOTOR_RIGHT].Current_Duty_Cycle = Motor_Parameters[MOTOR_RIGHT].Backward_Duty_Cycle;
			break;
	}
	
	// Store the new state, which is useful when the motor speed must change
	if (Motor == MOTOR_LEFT) Motor_Parameters[MOTOR_LEFT].Current_State = State;
	else Motor_Parameters[MOTOR_RIGHT].Current_State = State;
		
	MOTOR_PWM_TIMER_START();
}

// This function is unrolled by hand to prevent the compiler from using software multiplications in addresses calculation
void MotorChangeSpeed(TMotor Motor, TMotorDirection Direction, unsigned char Is_Speed_Increased)
{
	// The left motor is mounted in the opposite direction than the right motor
	if (Motor == MOTOR_LEFT)
	{
		if (Direction == MOTOR_DIRECTION_FORWARD)
		{
			if (Is_Speed_Increased)
			{
				if (Motor_Parameters[MOTOR_LEFT].Forward_Duty_Cycle > MOTOR_MINIMUM_DUTY_CYCLE_VALUE) Motor_Parameters[MOTOR_LEFT].Forward_Duty_Cycle--;
			}
			else
			{
				if (Motor_Parameters[MOTOR_LEFT].Forward_Duty_Cycle < MOTOR_MAXIMUM_DUTY_CYCLE_VALUE) Motor_Parameters[MOTOR_LEFT].Forward_Duty_Cycle++;
			}
			EEPROMWriteWord(MOTOR_LEFT_FORWARD_DUTY_CYCLE_EEPROM_ADDRESS, Motor_Parameters[MOTOR_LEFT].Forward_Duty_Cycle);
		}
		else
		{
			if (Is_Speed_Increased)
			{
				if (Motor_Parameters[MOTOR_LEFT].Backward_Duty_Cycle < MOTOR_MAXIMUM_DUTY_CYCLE_VALUE) Motor_Parameters[MOTOR_LEFT].Backward_Duty_Cycle++; // Motor is mounted in the opposite direction
			}
			else
			{
				if (Motor_Parameters[MOTOR_LEFT].Backward_Duty_Cycle > MOTOR_MINIMUM_DUTY_CYCLE_VALUE) Motor_Parameters[MOTOR_LEFT].Backward_Duty_Cycle--;
			}
			EEPROMWriteWord(MOTOR_LEFT_BACKWARD_DUTY_CYCLE_EEPROM_ADDRESS, Motor_Parameters[MOTOR_LEFT].Backward_Duty_Cycle);
		}
		
		// Set new speed
		MotorSetState(MOTOR_LEFT, Motor_Parameters[MOTOR_LEFT].Current_State);
	}
	else
	{
		if (Direction == MOTOR_DIRECTION_FORWARD)
		{
			if (Is_Speed_Increased)
			{
				if (Motor_Parameters[MOTOR_RIGHT].Forward_Duty_Cycle < MOTOR_MAXIMUM_DUTY_CYCLE_VALUE) Motor_Parameters[MOTOR_RIGHT].Forward_Duty_Cycle++;
			}
			else
			{
				if (Motor_Parameters[MOTOR_RIGHT].Forward_Duty_Cycle > MOTOR_MINIMUM_DUTY_CYCLE_VALUE) Motor_Parameters[MOTOR_RIGHT].Forward_Duty_Cycle--;
			}
			EEPROMWriteWord(MOTOR_RIGHT_FORWARD_DUTY_CYCLE_EEPROM_ADDRESS, Motor_Parameters[MOTOR_RIGHT].Forward_Duty_Cycle);
		}
		else
		{
			if (Is_Speed_Increased)
			{
				if (Motor_Parameters[MOTOR_RIGHT].Backward_Duty_Cycle > MOTOR_MINIMUM_DUTY_CYCLE_VALUE) Motor_Parameters[MOTOR_RIGHT].Backward_Duty_Cycle--;
			}
			else
			{
				if (Motor_Parameters[MOTOR_RIGHT].Backward_Duty_Cycle < MOTOR_MAXIMUM_DUTY_CYCLE_VALUE) Motor_Parameters[MOTOR_RIGHT].Backward_Duty_Cycle++;
			}
			EEPROMWriteWord(MOTOR_RIGHT_BACKWARD_DUTY_CYCLE_EEPROM_ADDRESS, Motor_Parameters[MOTOR_RIGHT].Backward_Duty_Cycle);
		}
		
		// Set new speed
		MotorSetState(MOTOR_RIGHT, Motor_Parameters[MOTOR_RIGHT].Current_State);
	}
}

// This function is unrolled by hand to prevent the compiler from using software multiplications in addresses calculation
unsigned short MotorReadSpeed(TMotor Motor, TMotorDirection Direction)
{
	if (Motor == MOTOR_LEFT)
	{
		if (Direction == MOTOR_DIRECTION_FORWARD) return Motor_Parameters[MOTOR_LEFT].Forward_Duty_Cycle;
		else return Motor_Parameters[MOTOR_LEFT].Backward_Duty_Cycle;
	}
	else
	{
		if (Direction == MOTOR_DIRECTION_FORWARD) return Motor_Parameters[MOTOR_RIGHT].Forward_Duty_Cycle;
		else return Motor_Parameters[MOTOR_RIGHT].Backward_Duty_Cycle;
	}
}
