package robot;

import java.io.IOException;

import network.NetworkClient;

/** The Odyssey project Mercury robot network interface.
 * @author Adrien RICCIARDI
 * @version 1.0 07/04/2014 
 */
public class Mercury
{
	/** Stop moving. */
	private final int COMMAND_STOP = 0;
    /** Go forward. */
	private final int COMMAND_FORWARD = 1;
	/** Go backward. */
    private final int COMMAND_BACKWARD = 2;
    /** Turn left. */
    private final int COMMAND_LEFT = 3;
    /** Turn right. */
    private final int COMMAND_RIGHT = 4;
    /** Ask for battery voltage. */
    private final int COMMAND_READ_BATTERY_VOLTAGE = 5;
    /** Light the Led. */
	private final int COMMAND_LED_ON = 6;
    /** Turn the Led off. */
	private final int COMMAND_LED_OFF = 7;
	/** Turn the robot off. */
	private final int COMMAND_POWER_OFF = 8;
	
	/** The wrapper used to communicate with the robot. */
	private NetworkClient _networkClient;
	
	/** Create a new access to the robot.
	 * @param robotIpAddress The robot IP address.
	 * @param robotPort The robot port.
	 */
	public Mercury(String robotIpAddress, int robotPort) throws Exception
	{
		_networkClient = new NetworkClient(robotIpAddress, robotPort);
	}
	
	/** Stop the robot. */
	public synchronized void stop() throws IOException
	{
		_networkClient.sendInteger(COMMAND_STOP);
	}
	
	/** Make the robot move forward. */
	public synchronized void moveForward() throws IOException
	{
		_networkClient.sendInteger(COMMAND_FORWARD);
	}
	
	/** Make the robot move backward. */
	public synchronized void moveBackward() throws IOException
	{
		_networkClient.sendInteger(COMMAND_BACKWARD);
	}
	
	/** Make the robot turn left. */
	public synchronized void turnLeft() throws IOException
	{
		_networkClient.sendInteger(COMMAND_LEFT);
	}
	
	/** Make the robot turn right. */
	public synchronized void turnRight() throws IOException
	{
		_networkClient.sendInteger(COMMAND_RIGHT);
	}
	
	/** Get the battery voltage and charge percentage.
	 * @return The battery voltage and percentage.
	 */
	public synchronized BatteryCharge getBatteryCharge() throws IOException
	{
		// Send robot command
		_networkClient.sendInteger(COMMAND_READ_BATTERY_VOLTAGE);
		
		// Get percentage value
		int percentage = _networkClient.receiveByte();
		
		// Get voltage value (it is stored as an integer value to avoid sending strange floating representations over the network) 
		int voltageRounded = _networkClient.receiveInteger();
		float voltage = (float) voltageRounded / 100.0f;
		
		return new BatteryCharge(percentage, voltage);
	}
	
	/** Light or turn off the Led.
	 * @param isLedOn Set to true to light the Led or to false to turn it off.
	 */
	public synchronized void setLedState(boolean isLedOn) throws IOException
	{
		if (isLedOn) _networkClient.sendInteger(COMMAND_LED_ON);
		else _networkClient.sendInteger(COMMAND_LED_OFF);	
	}
	
	/** Power off the robot. */
	public synchronized void powerOff() throws IOException
	{
		_networkClient.sendInteger(COMMAND_POWER_OFF);
	}
}
