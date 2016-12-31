/** @file Main.c
 * Read data from standard input and transmit it on the network. Next version will use UDP to transmit faster (UDP datagrams will contain a sequence number to be reconstituted in the right order on reception, and will be dropped after 100ms).
 * @author Adrien RICCIARDI
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Network.h"

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** The data buffer size in bytes. */
#define MAIN_BUFFER_SIZE 1024

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	unsigned short Port;
	int Server_Socket, Client_Socket;
	ssize_t Read_Bytes_Count, Written_Bytes_Count;
	unsigned char Buffer[MAIN_BUFFER_SIZE];
	
	// Check parameters
	if (argc != 2)
	{
		printf("Usage : %s Listening_Port\n"
			"  Listening_Port : the port the server will be bound to\n", argv[0]);
		return EXIT_FAILURE;
	}
	Port = atoi(argv[1]);
	
	// Run the server in background
/*	if (daemon(1, 1) != 0)
	{
		printf("Error : can't daemonize server.\n");
		return EXIT_FAILURE;
	}*/
	
	// Bind the server
	Server_Socket = NetworkServerCreate("0.0.0.0", Port);
	if (Server_Socket < 0)
	{
		printf("Error : failed to bind the server.\n");
		return EXIT_FAILURE;
	}
	
	// Wait for a client
	while (1)
	{
		Client_Socket = NetworkServerListen(Server_Socket);
		if (Client_Socket < 0)
		{
			printf("Error : failed to accept a client.\n");
			continue;
		}
		
		// Read data from stdin and send it over the network
		while (1)
		{
			// Get some data
			Read_Bytes_Count = read(STDIN_FILENO, Buffer, MAIN_BUFFER_SIZE);
			if (Read_Bytes_Count > 0)
			{
				Written_Bytes_Count = write(Client_Socket, Buffer, Read_Bytes_Count); // Send it over the network
				if (Written_Bytes_Count != Read_Bytes_Count) // An error happened, disconnect the client
				{
					close(Client_Socket);
					break;
				}
			}
		}
	}
}