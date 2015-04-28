/** @file Driver_ADC.h
 * Driver for the 10-bit Analog to Digital Converter.
 * @author Adrien RICCIARDI
 * @version 1.0 : 16/12/2013
 * @version 1.1 : 28/04/2015, used an interrupt to avoid hanging the system for too much time.
 */
#ifndef H_DRIVER_ADC_H
#define H_DRIVER_ADC_H

//--------------------------------------------------------------------------------------------------
// Functions
//--------------------------------------------------------------------------------------------------
/** Initialize ADC module to sample the battery voltage present on pin RA0.
 * @warning This function must be called only once at the beginning of the program.
 */
void ADCInitialize(void);

/** Start sampling the battery voltage. An interrupt is generated when the sampling has terminated. Use ADCReadLastSample() to retrieve the sampled value. */
void ADCStartSampling(void);

/** Get the last sampled value.
 * @return The last sampled battery voltage value.
 */
unsigned short ADCReadLastSample(void);

/** Check if the ADC interrupt happened.
 * @return 1 if the interrupt has triggered or 0 if not.
 */
#ifdef DOXYGEN
	unsigned char ADCHasInterruptOccured(void);
#else
	#define ADCHasInterruptOccured() pir1.ADIF
#endif

/** Clear interrupt flag to allow new interrupt. */
#ifdef DOXYGEN
	void ADCClearInterruptFlag(void);
#else
	#define ADCClearInterruptFlag() pir1.ADIF = 0
#endif

#endif