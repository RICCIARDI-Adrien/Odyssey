/** @file Main.c
 * Main loop for unsecure version.
 * @author Adrien RICCIARDI
 * @version 1.0 : 07/04/2014
 * @version 1.1 : 28/09/2014, added a command to power off the system.
 */
#include <arpa/inet.h> // For ntohl()
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h> // For atoi()
#include <sys/socket.h> // For recv()
#include <sys/types.h> // For recv()
#include <unistd.h> // For read(), write()...
#include "Log.h"
#include "Network.h"
#include "Robot.h"

//-------------------------------------------------------------------------------------------------
// Private variables
//-------------------------------------------------------------------------------------------------
static int Socket_Server, Socket_Client;
static unsigned char Battery_Voltage_Percentage = 0;
static unsigned int Battery_Voltage_Rounded = 0;
static char *String_Server_IP;

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Read the battery voltage each second. */
static void *ThreadReadBatteryVoltage(void __attribute__((unused)) *Pointer_Parameters)
{
	float Battery_Voltage;
	int Percentage;

	while (1)
	{
		// Compute percentage
		Battery_Voltage = RobotReadBatteryVoltage();
		Percentage = ((Battery_Voltage - ROBOT_MINIMUM_BATTERY_VOLTAGE) / (ROBOT_MAXIMUM_BATTERY_VOLTAGE - ROBOT_MINIMUM_BATTERY_VOLTAGE)) * 100.f;

		// Adjust values to stay in [0, 100] range
		if (Percentage < 0) Percentage = 0;
		else if (Percentage > 100) Percentage = 100;
		Battery_Voltage_Percentage = Percentage;

		// Multiply floating voltage value by 100 to keep 2 digits after the decimal
 		Battery_Voltage_Rounded = htonl(Battery_Voltage * 100.0f);

		usleep(1000000);
	}

	// Only to make gcc happy
	return NULL;
}

/** Called when the SIGTERM signal is received, stop the server.
 * @param Signal_Number The signal which triggered the handler.
 */
static void SignalHandlerTerm(int __attribute__((unused)) Signal_Number)
{
	close(Socket_Client);
	close(Socket_Server);
	Log("Server successfully exited.\n");
	exit(0);
}

/** Called when the SIGPIPE signal is received, stop the server (this signal tells that the connection dropped).
 * @param Signal_Number The signal which triggered the handler.
 */
static void SignalHandlerPipe(int __attribute__((unused)) Signal_Number)
{
	close(Socket_Client);
	Log("Client connection dropped, closing port.\n");
}

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	pthread_t Thread_ID;
	unsigned short Server_Port;
	TRobotCommand Command;
	struct sigaction Signal_Action;
	char *String_Log_File_Name;

	// Check parameters
	if (argc != 4)
	{
		printf("Usage : %s serverIpAddress serverListeningPort logFileName\n"
			"  serverIpAddress : dotted IPv4 address\n"
			"  serverListeningPort : decimal port value\n"
			"  logFileName : the file to store log messages into (use /dev/null to disable logs)\n", argv[0]);
		return EXIT_FAILURE;
	}

	// Retrieve parameters values
	String_Server_IP = argv[1];
	Server_Port = atoi(argv[2]);
	String_Log_File_Name = argv[3];
	
	// Start logging system
	if (LogInitialize(String_Log_File_Name) != 0)
	{
		printf("Error : failed to open the log file '%s'.\n", String_Log_File_Name);
		return EXIT_FAILURE;
	}
	Log("--- Network Control server starting ---\n");

	// Daemonize server soon so the created resources (threads) are available for the daemonized child and not for the father process
	if (daemon(1, 1) != 0)
	{
		Log("Error : can't daemonize server.\n");
		return EXIT_FAILURE;
	}

	// Connect to the robot
	if (!RobotInit("/dev/ttyAMA0"))
	{
		Log("Error : can't connect to the robot.\n");
		return EXIT_FAILURE;
	}
	// Stop robot in case of UART glitch
	RobotSetMotion(ROBOT_MOTION_STOPPED);
	RobotSetLedState(0);

	// Create battery voltage thread
	if (pthread_create(&Thread_ID, NULL, ThreadReadBatteryVoltage, NULL) != 0)
	{
		Log("Error : can't create battery voltage thread.\n");
		return EXIT_FAILURE;
	}

	// Create server
	Socket_Server = NetworkServerCreate(String_Server_IP, Server_Port);
	if (Socket_Server == -1)
	{
		Log("Error : can't bind server socket.\n");
		return EXIT_FAILURE;
	}
	else if (Socket_Server == -2)
	{
		Log("Error : can't bind server.\n");
		return EXIT_FAILURE;
	}

	// Set a new signal handler for SIGTERM, which is sent to stop the daemon
	Signal_Action.sa_handler = SignalHandlerTerm;
	if (sigaction(SIGTERM, &Signal_Action, NULL) == -1)
	{
		Log("Error : can't register term signal handler.\n");
		return EXIT_FAILURE;
	}

	// Set a new signal handler for SIGPIPE, which is sent when a socket link breaks
	Signal_Action.sa_handler = SignalHandlerPipe;
	if (sigaction(SIGPIPE, &Signal_Action, NULL) == -1)
	{
		Log("Error : can't register pipe signal handler.\n");
		return EXIT_FAILURE;
	}

	Log("Server ready.\n");

	while (1)
	{
		Log("Server waiting for client...\n");

		// Wait for the unique client
		Socket_Client = NetworkServerListen(Socket_Server);
		if (Socket_Client < 0)
		{
			Log("Error : the client could not connect.\n");
			continue;
		}
		Log("Client trying to connect...\n");

		while (1)
		{
			// Wait for a command
			if (recv(Socket_Client, &Command, sizeof(Command), MSG_WAITALL) != sizeof(Command))
			{
				Log("Could not receive client's command, disconnecting.\n");
				RobotSetMotion(ROBOT_MOTION_STOPPED);
				RobotSetLedState(0);
				close(Socket_Client);
				break;
			}
			// Convert big-endian network value to current system endianness
			Command = (TRobotCommand) ntohl(Command);

			// Execute command
			switch (Command)
			{
				case ROBOT_COMMAND_STOP:
					Log("Stop.\n");
					RobotSetMotion(ROBOT_MOTION_STOPPED);
					break;

				case ROBOT_COMMAND_FORWARD:
					Log("Forward.\n");
					RobotSetMotion(ROBOT_MOTION_FORWARD);
					break;

				case ROBOT_COMMAND_BACKWARD:
					Log("Backward.\n");
					RobotSetMotion(ROBOT_MOTION_BACKWARD);
					break;

				case ROBOT_COMMAND_LEFT:
					Log("Left.\n");
					RobotSetMotion(ROBOT_MOTION_FORWARD_TURN_LEFT);
					break;

				case ROBOT_COMMAND_RIGHT:
					Log("Right.\n");
					RobotSetMotion(ROBOT_MOTION_FORWARD_TURN_RIGHT);
					break;

				case ROBOT_COMMAND_READ_BATTERY_VOLTAGE:
					Log("Read battery voltage percentage : %d%%.\n", Battery_Voltage_Percentage);
					if (write(Socket_Client, &Battery_Voltage_Percentage, sizeof(Battery_Voltage_Percentage)) != sizeof(Battery_Voltage_Percentage)) Log("Warning : could not send battery voltage percentage correctly.\n");
					if (write(Socket_Client, &Battery_Voltage_Rounded, sizeof(Battery_Voltage_Rounded)) != sizeof(Battery_Voltage_Rounded)) Log("Warning : could not send battery voltage correctly.\n");
					break;

				case ROBOT_COMMAND_LED_ON:
					Log("Light led.\n");
					RobotSetLedState(1);
					break;

				case ROBOT_COMMAND_LED_OFF:
					Log("Turn off led.\n");
					RobotSetLedState(0);
					break;

				case ROBOT_COMMAND_POWER_OFF:
					Log("Halting the system.\n");
					if (system("sudo poweroff") == -1) Log("Failed to halt the system.\n");
					break;

				default:
					Log("Warning : unknown command received.\n");
					break;
			}
		}
	}
}
