/**
 * @page The Arduino Library for the proximity and ambient light sensor PALS-2
 * @section introduction Introduction
 * The infineon PALS-2 (packaged by Vishay as VCNL4135X01) is a proximity and ambient light
 * sensor. It offers proximity and ambient light readings with 16-bit resolution. I2C protocol
 * is used to communicate with the host microcontroller. It can be used for gesture recognition,
 * touch screen locking and dimming of displays.
 *
 * For the proximity function there are a built-in IRED driver and photo-pin-diode. LED driver
 * current can be programmed and up to 3 external IREDs can be connected. Offset compensation can be
 * enabled for the proximity measurement; with this feature the sensor writes the difference between
 * the normal proximity value and the estimated offset into the corresponding register.
 *
 * For the ambient light function there a one photo-pin-diode. Two additional photodiodes can receive
 * light in the blue area.
 *
 * Other features include: readouts either periodically or on-demand; interrupts for both functions, with
 * adjustable lower/upper thresholds and persistence.
 */

#ifndef PALS2_H_INCLUDED
#define PALS2_H_INCLUDED

#define PALS2_NUM_REG 			27
#define PALS2_REG_SIZE			1
#define SLAVE_ADDRESS 			0x13
#define NUM_MEASUREMENTS		4
#define NUM_MEASUREMENTS_BLUE	10

#define COMMAND_REGISTER		0x80
#define PROXIMITY_CONFIG		0x82
#define IRED_CONFIG				0x83
#define ALS_CONFIG				0x84
#define ALS_COMPENSATION		0x85
#define PROXIMITY_HIGH_BYTE		0x86
#define PROXIMITY_LOW_BYTE		0x87
#define ALS_HIGH_BYTE			0x88
#define ALS_LOW_BYTE			0x89
#define BLUE1_HIGH_BYTE			0x8C
#define BLUE1_LOW_BYTE			0x8D
#define BLUE2_HIGH_BYTE			0x8E
#define BLUE2_LOW_BYTE			0x8F
#define INTERRUPT_CONFIG		0x90

#define PROX_INT_BOT_HB			0x92
#define PROX_INT_BOT_LB			0x93
#define PROX_INT_TOP_HB			0x94
#define PROX_INT_TOP_LB			0x95
#define ALS_INT_BOT_HB			0x96
#define ALS_INT_BOT_LB			0x97
#define ALS_INT_TOP_HB			0x98
#define ALS_INT_TOP_LB			0x99

#include <Arduino.h>
#include <Wire.h>

class Pals2 {
public:
	Pals2();

	/**
	 * @brief Starts the sensor.
	 */
	void begin(void);

	/**
	 * @brief Enables periodic measurements of proximity and ambient light values.
	 */
	void enablePeriodicMeasurements(void);

	/**
	 * @brief Updates measurement data. Needed to be called in each measurement cycle.
	 */
	void updateData(void);

	/**
	 * @brief Gets sensor measurement updates. Should be called after \ref updateData(void) updateData().
	 * @return raw proximity value as an integer from 0 to 65536.
	 */
	uint16_t getRawProximity(void);

	/**
	 * @brief Gets sensor measurement updates. Should be called after \ref updateData(void) updateData()
	 * @return raw ambient light value as an integer from 0 to 65536.
	 */
	uint16_t getRawAmbientLight(void);

	/**
	 * @return a single raw proximity value measured on demand.
	 */
	uint16_t getRawProximityOnDemand(void);

	/**
	 * @return a single raw ambient light value measured on demand.
	 */
	uint16_t getRawAmbientLightOnDemand(void);

	/**
	 * @return the illuminance value computed from ALS and blue photodiode values.
	 */
	float getIlluminance(void);

	/**
	 * @brief Enables proximity offset compensation. The raw proximity values read will be the difference between the actual measured value and the estimated offset value,
	 * 		  thus 2 measurements are taken in each cycle. Works for both periodic and on-demand measurement.
	 */
	void enableProximityOffsetCompensation(void);

	/**
	 * @brief Disables proximity offset compensation.
	 */
	void disableProximityOffsetCompensation(void);

	/**
	 * @brief Sets the measurement rate of proximity measurement.
	 * @param rate Number of measurements per second. Can be one of the numbers from [2, 4, 8, 16, 32, 64, 128, 256].
	 */
	void setProximityMeasurementRate(uint16_t rate);

	/**
	 * @brief Sets the number of consecutive measurements needed above/below the threshold for an interrupt to be generated.
	 * @param persistence Number of valid measurements needed, which is one of the numbers from [1, 2, 4, 8, 16, 32, 64, 128].
	 */
	void setInterruptPersistence(uint8_t persistence);

	/**
	 * @brief Enables interrupts for proximity measurement and sets the lower/upper thresholds.
	 * @param topThreshold Upper threshold. By default 65536
	 * @param bottomThreshold Lower threshold. By default 0
	 */
	void enableProximityInterrupt(uint16_t topThreshold = 0xFF,
			uint16_t bottomThreshold = 0x00);

	/**
	 * @brief Disables interrupts for proximity measurement.
	 */
	void disableProximityInterrupt(void);

	/**
	 * @brief Enables interrupts for ambient light measurement and sets the lower/upper thresholds.
	 * @param topThreshold Upper threshold. By default 65536
	 * @param bottomThreshold Lower threshold. By default 0
	 */
	void enableAmbientLightInterrupt(uint16_t topThreshold = 0xFF,
			uint16_t bottomThreshold = 0x00);

	/**
	 * @brief Disables interrupts for ambient light measurement.
	 */
	void disableAmbientLightInterrupt(void);

	/**
	 * @brief For light sources with high intensity color compensation should be enabled (additional reading of blue PD will be conducted).
	 * @param colorCompPeriod The period of color compensation measurement; 0 for a shorter period (0 to 10ms) and 1 for a longer period (10 to 100ms)
	 */
	void enableColorCompensation(bool colorCompPeriod = 0);

	/**
	 * @brief Sets the ADC gain, which affects the calculation of illuminance. A higher ADC gain leads to a higher illuminance value.
	 * @param adcGain ADC gain in fA, can be 200/800/3200/25600; for any other value the default (200 fA) is taken
	 */
	void setADCGain(uint16_t adcGain);

	/**
	 * @brief Sets the rate of ambient light measurement.
	 * @alsRate Number of measurements per second, which is an integer from 1 to 8
	 */
	void setAmbientLightMeasurementRate(uint8_t alsRate);

	//TODO: set IRED (register #3)

	void resetSensor(void);
private:
	bool colorCompensationEnabled = false;
	uint8_t proximityConfig = 0;
	uint8_t ambientLightConfig = 0;
	uint8_t interruptConfig = 0;
	float gainFactor = 81.79;
	uint16_t rawProximity = 0;
	uint16_t rawAmbientLight = 0;
	float getBlueRatio(void);
	uint16_t blue1PD = 0;
	uint16_t blue2PD = 0;
	uint16_t concatResults(uint8_t upperByte, uint8_t lowerByte);
	void writeOut(uint16_t regNum, uint16_t val);
	void enableOnDemandReading(void);
};

#endif		/* PALS2_H_INCLUDED */
