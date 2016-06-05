/** @file Log.c
 * @see Log.h for description.
 * @author Adrien RICCIARDI
 */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "Log.h"

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
/** The log file. */
static FILE *Pointer_File_Log;

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Close the log file on program exit. */
static void LogExit(void)
{
	fclose(Pointer_File_Log);
}

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
int LogInitialize(char *String_Log_File_Name)
{
	// Try to open the file
	Pointer_File_Log = fopen(String_Log_File_Name, "w");
	if (Pointer_File_Log == NULL) return 1;
	
	// Automatically close the file when the program exits
	atexit(LogExit);
	
	return 0;
}

void Log(char *String_Format, ...)
{
	va_list List_Arguments;

	va_start(List_Arguments, String_Format);
	vfprintf(Pointer_File_Log, String_Format, List_Arguments);
	va_end(List_Arguments);
}
