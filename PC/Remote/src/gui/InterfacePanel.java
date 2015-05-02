package gui;

import java.awt.event.ActionListener;
import java.awt.event.KeyListener;
import javax.swing.*;

/** @class InterfacePanel
 * Display graphical interface and format user input data. 
 * @author Adrien RICCIARDI
 * @version 1.0 08/04/2014
 */
public class InterfacePanel extends JPanel
{
	private static final long serialVersionUID = 1L;
	private JTextField _textFieldIpAddress;
	private JTextField _textFieldPort;
	private JButton _buttonConnect;
	private JButton _buttonPowerOff;
	private JLabel _labelBattery;
	private JLabel _labelMotion;
	private JLabel _labelLed;
	private KeyListener _keyListener;
	
	/**
	 * Create the panel.
	 */
	public InterfacePanel()
	{
		setLayout(null);
		
		JLabel lblNewLabel = new JLabel("IP address :");
		lblNewLabel.setBounds(12, 12, 94, 15);
		add(lblNewLabel);
		
		JLabel lblNewLabel_1 = new JLabel("Port :");
		lblNewLabel_1.setBounds(12, 39, 70, 15);
		add(lblNewLabel_1);
		
		_textFieldIpAddress = new JTextField();
		_textFieldIpAddress.setText("192.168.100.1");
		_textFieldIpAddress.setBounds(124, 10, 114, 19);
		add(_textFieldIpAddress);
		_textFieldIpAddress.setColumns(10);
		
		_textFieldPort = new JTextField();
		_textFieldPort.setText("1234");
		_textFieldPort.setBounds(124, 37, 70, 19);
		add(_textFieldPort);
		_textFieldPort.setColumns(10);
		
		_buttonConnect = new JButton("Connect");
		_buttonConnect.setBounds(124, 68, 117, 25);
		add(_buttonConnect);
		
		_buttonPowerOff = new JButton("Power off");
		_buttonPowerOff.setBounds(250, 68, 117, 25);
		_buttonPowerOff.setEnabled(false);
		add(_buttonPowerOff);
		
		JSeparator separator = new JSeparator();
		separator.setBounds(0, 105, 400, 1);
		add(separator);
		
		_labelBattery = new JLabel("Battery : 0% (0V)");
		_labelBattery.setBounds(12, 118, 261, 15);
		add(_labelBattery);
		
		_labelMotion = new JLabel("Last motion : STOP");
		_labelMotion.setBounds(12, 145, 243, 15);
		add(_labelMotion);
		
		_labelLed = new JLabel("Led state : OFF");
		_labelLed.setBounds(12, 172, 159, 15);
		add(_labelLed);
		
		JLabel lblUseArrowKeys = new JLabel("Arrow keys : move");
		lblUseArrowKeys.setBounds(12, 219, 309, 15);
		add(lblUseArrowKeys);
		
		JLabel lblNewLabel_2 = new JLabel("Space : stop");
		lblNewLabel_2.setBounds(12, 238, 319, 15);
		add(lblNewLabel_2);
		
		JLabel lblFToToggle = new JLabel("L : toggle LED state");
		lblFToToggle.setBounds(12, 256, 182, 15);
		add(lblFToToggle);
		
		JSeparator separator_1 = new JSeparator();
		separator_1.setBounds(0, 206, 400, 1);
		add(separator_1);
	}
	
	/** Get the IP address field.
	 * @return The IP address or null if it not correct.
	 */
	public String getIpAddress()
	{
		String ipAddress = _textFieldIpAddress.getText();
		
		// Check for IP address correctness
		if (ipAddress.matches("^[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}$")) return ipAddress;
		return null;
	}
	
	/** Get the port field.
	 * @return The port value or -1 if it is not correct.
	 */
	public int getPort()
	{
		int port = 0;
		
		// Convert port to its binary representation
		try
		{
			port = Integer.parseInt(_textFieldPort.getText());
		}
		catch (NumberFormatException exception)
		{
			return -1;
		}
		
		// Is the port in bounds ?
		if ((port < 0) || (port > 65535)) return -1;
		return port;
	}
	
	/** Enable the graphical components allowing to connect to the robot. */
	public void enableConnectionInterface()
	{
		_textFieldIpAddress.setEnabled(true);
		_textFieldPort.setEnabled(true);
		_buttonConnect.setEnabled(true);
		_buttonPowerOff.setEnabled(false);
	}
	
	/** Disable the graphical components allowing to connect to the robot. */
	public void disableConnectionInterface()
	{
		_textFieldIpAddress.setEnabled(false);
		_textFieldPort.setEnabled(false);
		_buttonConnect.setEnabled(false);
		_buttonPowerOff.setEnabled(true);
	}
	
	/** Assign a listener to the Connect button.
	 * @param listener The listener to use.
	 */
	public void setConnectButtonListener(ActionListener listener)
	{
		_buttonConnect.addActionListener(listener);
	}
	
	/** Assign a listener to the Power off button.
	 * @param listener The listener to use.
	 */
	public void setPowerOffButtonListener(ActionListener listener)
	{
		_buttonPowerOff.addActionListener(listener);
	}
	
	/** Assign a listener to the Interface panel.
	 * @param listener The listener to use.
	 */
	public void setInterfacePanelListener(KeyListener listener)
	{
		setFocusable(true);
		addKeyListener(listener);
		_keyListener = listener;
	}
	
	/** Remove any listener from the Interface panel. */
	public void removeInterfacePanelListener()
	{
		setFocusable(false);
		removeKeyListener(_keyListener);
	}
	
	/** Display the battery percentage and voltage values.
	 * @param percentage The battery percentage.
	 * @param voltage The battery voltage.
	 */
	public void setBatteryLabel(int percentage, float voltage)
	{
		_labelBattery.setText("Battery : " + percentage + "% (" + voltage + "V)"); 
	}
	
	/** Display the last robot motion.
	 * @param motion The motion.
	 */
	public void setMotionLabel(String motion)
	{
		_labelMotion.setText("Last motion : " + motion); 
	}
	
	/** Tell the current state of the Led.
	 * @param isLedOn Set to true to tell that the Led is lighted or set to false to tell it is turned off.
	 */
	public void setLedLabel(boolean isLedOn)
	{
		String state;
		if (isLedOn) state = "ON";
		else state = "OFF";
		
		_labelLed.setText("Led state : " + state);
	}
}
