/** @file Interface.c
 * @see Interface.h for description.
 * @author Adrien RICCIARDI
 */
#include <curses.h>
#include <stdio.h>
#include <string.h>
#include "Interface.h"

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** Ncurses yellow color pair. */
#define INTERFACE_COLOR_PAIR_YELLOW 1
/** Ncurses red color pair. */
#define INTERFACE_COLOR_PAIR_RED 2

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
int InterfaceInit(void)
{
	// Initialize curses
	if (initscr() == NULL) return 0;

	// Save current terminal screen
	savetty();

	// Enable use of colors
	start_color();
	// Create all colors used by the program
	init_pair(INTERFACE_COLOR_PAIR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(INTERFACE_COLOR_PAIR_RED, COLOR_RED, COLOR_BLACK);

	// Disable terminal line buffering to get each character when user types it
	if (cbreak() == ERR)
	{
		InterfaceTerminate();
		return 0;
	}

	// Disable characters echo
	if (noecho() == ERR)
	{
		InterfaceTerminate();
		return 0;
	}

	// Hide cursor
	curs_set(0);

	return 1;
}

void InterfaceTerminate(void)
{
	// Restore previous terminal screen
	resetty();

	// Stop ncurses
	endwin();
}

void InterfaceDisplayBatteryVoltage(int Percentage, float Voltage)
{
	char String_Output[32];
	int Is_Battery_Low;

	if (Percentage < 30) Is_Battery_Low = 1;
	else Is_Battery_Low = 0;

	// Format values (the extra spaces at the end of the line are used to erase a longer line previously displayed)
	sprintf(String_Output, "Battery : %d%% (%.2f V)     ", Percentage, Voltage);

	// Display values
	if (Is_Battery_Low) attron(COLOR_PAIR(INTERFACE_COLOR_PAIR_RED));
	mvaddstr(0, 0, String_Output);
	if (Is_Battery_Low) attroff(COLOR_PAIR(INTERFACE_COLOR_PAIR_RED));
	refresh();
}

void InterfaceDisplayDirection(char *String_Direction)
{
	// Clear previous trace
	mvaddstr(2, 0, "                            ");

	// Show current direction
	mvaddstr(2, 0, "Direction : ");
	attron(COLOR_PAIR(INTERFACE_COLOR_PAIR_YELLOW));
	addstr(String_Direction);
	attroff(COLOR_PAIR(INTERFACE_COLOR_PAIR_YELLOW));
	refresh();
}

void InterfaceDisplaySpeeds(int Left_Motor_Forward_Speed, int Left_Motor_Backward_Speed, int Right_Motor_Forward_Speed, int Right_Motor_Backward_Speed)
{
	const float PWM_Period = 4.44f, PWM_Precision = 1023.0f;
	char String_Speed[32];
	float Percentage, PWM_High_Period;

	// Left motor forward speed
	mvaddstr(4, 0, "Left forward speed   : ");
	Percentage = (100.0f * Left_Motor_Forward_Speed) / PWM_Precision;
	PWM_High_Period = (PWM_Period * Percentage) / 100.0f;
	sprintf(String_Speed, "%d (%d%%, %.2f ms)", Left_Motor_Forward_Speed, (int) Percentage, PWM_High_Period);
	attron(COLOR_PAIR(INTERFACE_COLOR_PAIR_YELLOW));
	addstr(String_Speed);
	attroff(COLOR_PAIR(INTERFACE_COLOR_PAIR_YELLOW));
	addstr("      "); // Erase previous trace

	// Right motor forward speed
	mvaddstr(5, 0, "Right forward speed  :  ");
	Percentage = (100.0f * Right_Motor_Forward_Speed) / PWM_Precision;
	PWM_High_Period = (PWM_Period * Percentage) / 100.0f;
	sprintf(String_Speed, "%d (%d%%, %.2f ms)", Right_Motor_Forward_Speed, (int) Percentage, PWM_High_Period);
	attron(COLOR_PAIR(INTERFACE_COLOR_PAIR_YELLOW));
	addstr(String_Speed);
	attroff(COLOR_PAIR(INTERFACE_COLOR_PAIR_YELLOW));
	addstr("      "); // Erase previous trace

	// Left motor backward speed
	mvaddstr(6, 0, "Left backward speed  : ");
	Percentage = (100.0f * Left_Motor_Backward_Speed) / PWM_Precision;
	PWM_High_Period = (PWM_Period * Percentage) / 100.0f;
	sprintf(String_Speed, "%d (%d%%, %.2f ms)", Left_Motor_Backward_Speed, (int) Percentage, PWM_High_Period);
	attron(COLOR_PAIR(INTERFACE_COLOR_PAIR_YELLOW));
	addstr(String_Speed);
	attroff(COLOR_PAIR(INTERFACE_COLOR_PAIR_YELLOW));
	addstr("      "); // Erase previous trace

	// Right motor backward speed
	mvaddstr(7, 0, "Right backward speed  :  ");
	Percentage = (100.0f * Right_Motor_Backward_Speed) / PWM_Precision;
	PWM_High_Period = (PWM_Period * Percentage) / 100.0f;
	sprintf(String_Speed, "%d (%d%%, %.2f ms)", Right_Motor_Backward_Speed, (int) Percentage, PWM_High_Period);
	attron(COLOR_PAIR(INTERFACE_COLOR_PAIR_YELLOW));
	addstr(String_Speed);
	attroff(COLOR_PAIR(INTERFACE_COLOR_PAIR_YELLOW));
	addstr("      "); // Erase previous trace

	refresh();
}

void InterfaceDisplayHints(void)
{
	char *String_Hints = "Z, Q, S, D : move the robot.\n" \
		"Space : stop the robot.\n" \
		"U, J : increase/decrease left motor forward speed.\n" \
		"I, K : increase/decrease right motor forward speed.\n" \
		"O, L : increase/decrease left motor backward speed.\n" \
		"P, M : increase/decrease right motor backward speed.\n" \
		"X : exit program.";

	mvaddstr(LINES - 7, 0, String_Hints);
	refresh();
}
