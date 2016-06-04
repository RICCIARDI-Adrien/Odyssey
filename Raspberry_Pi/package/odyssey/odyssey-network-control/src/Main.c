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
	Log(LOG_INFO, "Server successfully exited.");
	exit(0);
}

/** Called when the SIGPIPE signal is received, stop the server (this signal tells that the connection dropped).
 * @param Signal_Number The signal which triggered the handler.
 */
static void SignalHandlerPipe(int __attribute__((unused)) Signal_Number)
{
	close(Socket_Client);
	Log(LOG_INFO, "Client connection dropped, closing port.");
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

	// Connect to syslog
	LogInit("Network Control");
	Log(LOG_INFO, "--- Network Control server starting ---");

	// Check parameters
	if (argc != 3)
	{
		Log(LOG_ERR, "Bad calling arguments. Usage : %s serverIpAddress serverListeningPort", argv[0]);
		return EXIT_FAILURE;
	}

	// Retrieve parameters values
	String_Server_IP = argv[1];
	Server_Port = atoi(argv[2]);

	// Daemonize server soon so the created resources (threads) are made for the daemonized child and not for the father process
	if (daemon(1, 1) != 0)
	{
		Log(LOG_ERR, "Error : can't daemonize server.");
		return EXIT_FAILURE;
	}

	// Connect to the robot
	if (!RobotInit("/dev/ttyAMA0"))
	{
		Log(LOG_ERR, "Error : can't connect to the robot.");
		return EXIT_FAILURE;
	}
	// Stop robot in case of UART glitch
	RobotSetMotion(ROBOT_MOTION_STOPPED);
	RobotSetLedState(0);

	// Create battery voltage thread
	if (pthread_create(&Thread_ID, NULL, ThreadReadBatteryVoltage, NULL) != 0)
	{
		Log(LOG_ERR, "Error : can't create battery voltage thread.");
		return EXIT_FAILURE;
	}

	// Create server
	Socket_Server = NetworkServerCreate(String_Server_IP, Server_Port);
	if (Socket_Server == -1)
	{
		Log(LOG_ERR, "Error : can't bind server socket.");
		return EXIT_FAILURE;
	}
	else if (Socket_Server == -2)
	{
		Log(LOG_ERR, "Error : can't bind server.");
		return EXIT_FAILURE;
	}

	// Set a new signal handler for SIGTERM, which is sent to stop the daemon
	Signal_Action.sa_handler = SignalHandlerTerm;
	if (sigaction(SIGTERM, &Signal_Action, NULL) == -1)
	{
		Log(LOG_ERR, "Error : can't register term signal handler.");
		return EXIT_FAILURE;
	}

	// Set a new signal handler for SIGPIPE, which is sent when a socket link breaks
	Signal_Action.sa_handler = SignalHandlerPipe;
	if (sigaction(SIGPIPE, &Signal_Action, NULL) == -1)
	{
		Log(LOG_ERR, "Error : can't register pipe signal handler.");
		return EXIT_FAILURE;
	}

	Log(LOG_INFO, "Server ready.");

	while (1)
	{
		Log(LOG_INFO, "Server waiting for client...");

		// Wait for the unique client
		Socket_Client = NetworkServerListen(Socket_Server);
		if (Socket_Client < 0)
		{
			Log(LOG_ERR, "Error : the client could not connect.");
			continue;
		}
		Log(LOG_INFO, "Client trying to connect...");

		while (1)
		{
			// Wait for a command
			if (recv(Socket_Client, &Command, sizeof(Command), MSG_WAITALL) != sizeof(Command))
			{
				Log(LOG_INFO, "Could not receive client's command, disconnecting.");
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
					Log(LOG_DEBUG, "Stop.");
					RobotSetMotion(ROBOT_MOTION_STOPPED);
					break;

				case ROBOT_COMMAND_FORWARD:
					Log(LOG_DEBUG, "Forward.");
					RobotSetMotion(ROBOT_MOTION_FORWARD);
					break;

				case ROBOT_COMMAND_BACKWARD:
					Log(LOG_DEBUG, "Backward.");
					RobotSetMotion(ROBOT_MOTION_BACKWARD);
					break;

				case ROBOT_COMMAND_LEFT:
					Log(LOG_DEBUG, "Left.");
					RobotSetMotion(ROBOT_MOTION_FORWARD_TURN_LEFT);
					break;

				case ROBOT_COMMAND_RIGHT:
					Log(LOG_DEBUG, "Right.");
					RobotSetMotion(ROBOT_MOTION_FORWARD_TURN_RIGHT);
					break;

				case ROBOT_COMMAND_READ_BATTERY_VOLTAGE:
					Log(LOG_DEBUG, "Read battery voltage percentage : %d%%.", Battery_Voltage_Percentage);
					if (write(Socket_Client, &Battery_Voltage_Percentage, sizeof(Battery_Voltage_Percentage)) != sizeof(Battery_Voltage_Percentage)) Log(LOG_WARNING, "Warning : could not send battery voltage percentage correctly.");
					if (write(Socket_Client, &Battery_Voltage_Rounded, sizeof(Battery_Voltage_Rounded)) != sizeof(Battery_Voltage_Rounded)) Log(LOG_WARNING, "Warning : could not send battery voltage correctly.");
					break;

				case ROBOT_COMMAND_LED_ON:
					Log(LOG_DEBUG, "Light led.");
					RobotSetLedState(1);
					break;

				case ROBOT_COMMAND_LED_OFF:
					Log(LOG_DEBUG, "Turn off led.");
					RobotSetLedState(0);
					break;

				case ROBOT_COMMAND_POWER_OFF:
					Log(LOG_DEBUG, "Halting the system.");
					if (system("sudo halt") == -1) Log(LOG_ERR, "Failed to halt the system.");
					break;

				default:
					Log(LOG_DEBUG, "Unknown command received.");
					break;
			}
		}
	}
}
