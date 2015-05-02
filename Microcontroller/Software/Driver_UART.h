/** @file Driver_UART.h
 * Driver for the RS-232 Universal Asynchronous Receiver Transmitter.
 * @author Adrien RICCIARDI
 * @version 1.0 : 15/12/2013
 * @version 1.1 : 03/03/2014, handled reception overrun error.
 */ 
#ifndef H_DRIVER_UART_H
#define H_DRIVER_UART_H

#include <system.h> // Needed to use registers names

//--------------------------------------------------------------------------------------------------
// Constants
//--------------------------------------------------------------------------------------------------
/** Baud rate of 115200 bit/s with a 3.6864 MHz clock. */
#define UART_BAUD_RATE_115200 1
/** Baud rate of 230400 bit/s with a 3.6864 MHz clock. */
#define UART_BAUD_RATE_230400 0

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize UART module at the specified baud rate.
 * @param Baud_Rate Use a UART_BAUD_RATE_X constant to indicate wanted baud rate.
 * @warning This function must be called only once at the beginning of the program.
 */
void UARTInitialize(unsigned char Baud_Rate);

/** Read a byte from the UART.
 * @return The read byte.
 * @note This function waits until a byte is available.
 */
unsigned char UARTReadByte(void);

/** Write a byte to the UART.
 * @param Byte The byte to write.
 */
void UARTWriteByte(unsigned char Byte);

#endif
