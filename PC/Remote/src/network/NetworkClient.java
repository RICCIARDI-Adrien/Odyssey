package network;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

/** @class NetworkClient
 * A simple bidirectional TCP connection.
 * @author Adrien RICCIARDI
 * @version 1.0 07/04/2014
 */
public class NetworkClient
{
	/** The socket used to communicate with the server. */
	private Socket _socket = null;
	
	/** Read data from the socket. */
	private DataInputStream _inputStream;
	/** Write data to the socket. */
	private DataOutputStream _outputStream;
	
	/** Connect a new TCP client to the specified server.
	 * @param serverIpAddress The server IP address.
	 * @param serverPort The server port.
	 */
	public NetworkClient(String serverIpAddress, int serverPort) throws Exception
	{
		// Connect to the server
		_socket = new Socket(serverIpAddress, serverPort);

		// Initialize accesses to the socket
		_inputStream = new DataInputStream(_socket.getInputStream());
		_outputStream = new DataOutputStream(_socket.getOutputStream());
	}
	
	/** Receive a big-endian 4-byte value from the network.
	 * @return The received integer.
	 */
	public int receiveInteger() throws IOException
	{
		return _inputStream.readInt();
	}
	
	/** Receive a 1-byte value from the network.
	 * @return The received byte.
	 */
	public byte receiveByte() throws IOException
	{
		return _inputStream.readByte();
	}
	
	/** Send a big-endian 4-byte value to the network.
	 * @param data The value to send.
	 */
	public void sendInteger(int data) throws IOException
	{
		_outputStream.writeInt(data);
	}
}
