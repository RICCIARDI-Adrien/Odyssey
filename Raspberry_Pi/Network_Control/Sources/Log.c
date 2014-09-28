/** @file Log.c
 * @see Log.h for description.
 * @author Adrien RICCIARDI
 */
#include <stdarg.h>
#include <syslog.h>
#include "Log.h"

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
#if LOG_ENABLE == 1
	void LogInit(char *String_Program_Name)
	{
		openlog(String_Program_Name, LOG_CONS, LOG_DAEMON);
	}

	void Log(int Priority, char *String_Format, ...)
	{
		va_list List_Arguments;

		va_start(List_Arguments, String_Format);
		vsyslog(Priority, String_Format, List_Arguments);
		va_end(List_Arguments);
	}
#endif


