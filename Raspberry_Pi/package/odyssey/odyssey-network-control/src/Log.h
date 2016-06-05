/** @file Log.h
 * Write log messages to a file.
 * @author Adrien RICCIARDI
 */
#ifndef H_LOG_H
#define H_LOG_H

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Open the log file.
 * @param String_Log_File_Name The file to store log messages to.
 * @return 0 on success,
 * @return 1 if an error happened.
 */
int LogInitialize(char *String_Log_File_Name);

/** Write a debug message to log system
 * @param String_Format Format string same than printf().
 */
void Log(char *String_Format, ...);

#endif

