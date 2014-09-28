package robot;

/** @class BatteryVoltage
 * Represent a battery charge by its percentage and its voltage.
 * @author Adrien RICCIARDI
 * @version 1.0 08/04/2014
 */
public class BatteryCharge
{
	/** Charge percentage. */
	private int _percentage;
	/** Voltage value. */
	private float _voltage;
	
	/** Create a new battery charge representation.
	 * @param percentage The percentage value.
	 * @param voltage The voltage value.
	 */
	public BatteryCharge(int percentage, float voltage)
	{
		_percentage = percentage;
		_voltage = voltage;
	}
	
	/** Get the battery charge percentage.
	 * @return The percentage.
	 */
	public int getPercentage()
	{
		return _percentage;
	}
	
	/** Get the battery voltage.
	 * @return The battery voltage.
	 */
	public float getVoltage()
	{
		return _voltage;
	}
}
