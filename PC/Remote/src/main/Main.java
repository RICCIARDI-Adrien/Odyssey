package main;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.IOException;

import javax.swing.*;

import gui.MainFrame;
import robot.BatteryCharge;
import robot.Mercury;

/** @class Main
 * Main loop.
 * @author Adrien RICCIARDI
 * @version 1.0 07/04/2014
 */
public class Main
{	
	/** The robot. */
	private static Mercury _robot = null;
	/** Handle the graphical user interface. */
	private static MainFrame _mainFrame;
	/** Tell if the user is successfully connected to the robot or not. */ 
	private static volatile boolean _isRobotConnected = false; // MUST be volatile or some cache will hide the value to the thread
	
	/** Reset the program when the robot has been somewhat disconnected.
	 * @param exceptionMessage The exception that caused the disconnection.
	 */
	private static void handleRobotDisconnection(String exceptionMessage)
	{
		_mainFrame.resetInterface();
		_isRobotConnected = false;
		JOptionPane.showMessageDialog(null, "Can't communicate with the robot (" + exceptionMessage + ").", "Error", JOptionPane.ERROR_MESSAGE);
	}
	
	/** Handle Connect button actions. */
	private static class ConnectButtonActionListener implements ActionListener
	{
		@Override
		public void actionPerformed(ActionEvent event)
		{
			// Get parameters
			String ipAddress = _mainFrame.getIpAddress();
			if (ipAddress == null)
			{
				JOptionPane.showMessageDialog(null, "Invalid IP address.", "Error", JOptionPane.ERROR_MESSAGE);
				return;
			}
			
			int port = _mainFrame.getPort();
			if (port == -1)
			{
				JOptionPane.showMessageDialog(null, "Invalid port value.", "Error", JOptionPane.ERROR_MESSAGE);
				return;
			}
			
			// Connect to the robot
			try
			{
				_robot = new Mercury(ipAddress, port);
			}
			catch (Exception exception)
			{
				JOptionPane.showMessageDialog(null, "Can't connect to the robot (" + exception.getMessage() + ").", "Error", JOptionPane.ERROR_MESSAGE);
				return;
			}
			
			_mainFrame.disableConnectionInterface();
			// Allow keystroke handling by the Interface panel
			_mainFrame.setInterfacePanelListener(new InterfacePanelKeyListener());

			_isRobotConnected = true;
			JOptionPane.showMessageDialog(null, "Successfully connected to the robot.", "Connection success", JOptionPane.INFORMATION_MESSAGE);
		}		
	}
	
	/** Handle Power off button actions. */
	private static class PowerOffButtonActionListener implements ActionListener
	{
		@Override
		public void actionPerformed(ActionEvent event)
		{
			try
			{
				_robot.powerOff();
			}
			catch (IOException exception)
			{
				handleRobotDisconnection(exception.getMessage());
			}
		}
	}
	
	
	/** Handle Interface panel keystrokes. */
	private static class InterfacePanelKeyListener implements KeyListener
	{
		/** The current Led state. */
		private boolean _isLedOn = false;
		
		@Override
		public void keyPressed(KeyEvent event)
		{
			try
			{
				switch (event.getKeyCode())
				{
					case KeyEvent.VK_SPACE:
						_robot.stop();
						_mainFrame.setMotionLabel("STOP");
						break;
					
					case KeyEvent.VK_UP:
						_robot.moveForward();
						_mainFrame.setMotionLabel("FORWARD");
						break;
						
					case KeyEvent.VK_DOWN:
						_robot.moveBackward();
						_mainFrame.setMotionLabel("BACKWARD");
						break;
						
					case KeyEvent.VK_LEFT:
						_robot.turnLeft();
						_mainFrame.setMotionLabel("LEFT");
						break;
						
					case KeyEvent.VK_RIGHT:
						_robot.turnRight();
						_mainFrame.setMotionLabel("RIGHT");
						break;
						
					case KeyEvent.VK_L:
						_isLedOn = !_isLedOn;
						_robot.setLedState(_isLedOn);
						_mainFrame.setLedLabel(_isLedOn);
						break;
						
					default:
						break;
				}
			}
			catch (IOException exception)
			{
				handleRobotDisconnection(exception.getMessage());
			}
		}

		@Override
		public void keyReleased(KeyEvent event) {}

		@Override
		public void keyTyped(KeyEvent event) {}
	}
	
	/** Ask for battery charge every second and display it. */ 
	private static class BatteryChargeThread extends Thread
	{
		public void run()
		{
			while (true)
			{
				if (_isRobotConnected)
				{
					// Read charge from the robot
					BatteryCharge charge = null;
					try
					{
						charge = _robot.getBatteryCharge();
					}
					catch (IOException exception)
					{
						handleRobotDisconnection(exception.getMessage());
						continue;
					}
					_mainFrame.setBatteryLabel(charge.getPercentage(), charge.getVoltage());
					
					try
					{
						Thread.sleep(1000, 0);
					}
					catch (Exception exception) {}
				}
			}
		}
	}
	
	public static void main(String args[])
	{
		// Display graphical interface
		_mainFrame = new MainFrame();
		_mainFrame.setConnectButtonListener(new ConnectButtonActionListener());
		_mainFrame.setPowerOffButtonListener(new PowerOffButtonActionListener());
		
		// Start battery charge thread
		BatteryChargeThread batteryChargeThread = new BatteryChargeThread();
		batteryChargeThread.start();
	}
}
