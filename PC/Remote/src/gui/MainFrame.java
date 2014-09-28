package gui;

import java.awt.Container;
import java.awt.event.ActionListener;
import java.awt.event.KeyListener;

import javax.swing.JFrame;

/** @class MainFrame
 * The program graphical interface.
 * @author Adrien RICCIARDI
 * @version 1.0 07/04/2014
 */
public class MainFrame extends JFrame
{
	/** To make Eclipse happy. */
	private static final long serialVersionUID = 1L;
	
	/** Frame width in pixels. */
	private final int MAIN_FRAME_WIDTH = 400;
	/** Frame height in pixels. */
	private final int MAIN_FRAME_HEIGHT = 305;
	
	/** The panel containing all the interface components. */
	private InterfacePanel _interfacePanel;
	
	/** Create the program main window. */
	public MainFrame()
	{
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		
		// Set frame layout
		Container contentPane = getContentPane();
		
		// Add panel
		_interfacePanel = new InterfacePanel();
		contentPane.add(_interfacePanel);
		
		// Configure frame size
		setSize(MAIN_FRAME_WIDTH, MAIN_FRAME_HEIGHT);
		setResizable(false);		
		
		setTitle("Remote");
		setVisible(true);
	}
	
	/** Get the IP address.
	 * @return The IP address;
	 */
	public String getIpAddress()
	{
		return _interfacePanel.getIpAddress();
	}
	
	/** Get the network port.
	 * @return The port.
	 */
	public int getPort()
	{
		return _interfacePanel.getPort();
	}
	
	/** Disable connection graphical components. */
	public void disableConnectionInterface()
	{
		_interfacePanel.disableConnectionInterface();
	}
	
	/** Assign a listener to the Connect button.
	 * @param listener The listener to use.
	 */
	public void setConnectButtonListener(ActionListener listener)
	{
		_interfacePanel.setConnectButtonListener(listener);
	}
	
	/** Assign a listener to the Power off button.
	 * @param listener The listener to use.
	 */
	public void setPowerOffButtonListener(ActionListener listener)
	{
		_interfacePanel.setPowerOffButtonListener(listener);
	}
	
	/** Assign a listener to the Interface panel.
	 * @param listener The listener to use.
	 */
	public void setInterfacePanelListener(KeyListener listener)
	{
		_interfacePanel.setInterfacePanelListener(listener);
	}
	
	/** Display the battery percentage and voltage values.
	 * @param percentage The battery percentage.
	 * @param voltage The battery voltage.
	 */
	public void setBatteryLabel(int percentage, float voltage)
	{
		_interfacePanel.setBatteryLabel(percentage, voltage); 
	}
	
	/** Display the last robot motion.
	 * @param motion The motion.
	 */
	public void setMotionLabel(String motion)
	{
		_interfacePanel.setMotionLabel(motion);
	}
	
	/** Tell the current state of the Led.
	 * @param isLedOn Set to true to tell that the Led is lighted or set to false to tell it is turned off.
	 */
	public void setLedLabel(boolean isLedOn)
	{
		_interfacePanel.setLedLabel(isLedOn);
	}
	
	/** Reset the Interface panel to initial state. */
	public void resetInterface()
	{
		// Disable panel keystroke handling
		_interfacePanel.removeInterfacePanelListener();
		
		// Reset displayed informations
		setBatteryLabel(0, 0);
		setMotionLabel("STOP");
		setLedLabel(false);
		
		// Allow user to reconnect to the robot
		_interfacePanel.enableConnectionInterface();
	}
	
}
