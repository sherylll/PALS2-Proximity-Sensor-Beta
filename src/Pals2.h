/**
 *  @page The Arduino Library for the proximity and ambient light sensor VCNL4531X01
 *  @section 
 */

#ifndef PALS2_H_INCLUDED
#define PALS2_H_INCLUDED

#define PALS2_NUM_REG 		27
#define PALS2_REG_SIZE		1
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
	void begin(void);
	void updateData(void);
	uint16_t getRawProximity(void);
	uint16_t getRawAmbientLight(void);
	uint16_t getRawProximityOnDemand(void);
	uint16_t getRawAmbientLightOnDemand(void);
	float getIlluminance(void);

	//If prox offset compensation is enabled the proximity measurement result register contain just the difference between total measured counts
	//and measured offset value. With this enabled prox offset always 2 measurements are carried out when starting either on demand or self-timed proximity measurements
	void enableProximityOffsetCompensation(void);
	void disableProximityOffsetCompensation(void);
	void setProximityMeasurementRate(uint16_t);

	void setInterruptPersistence(uint8_t persistence);
	void enableProximityInterrupt(uint16_t topThreshold, uint16_t bottomThreshold);
	void disableProximityInterrupt(void);

	void enableAmbientLightInterrupt(uint16_t topThreshold, uint16_t bottomThreshold);
	void disableAmbientLightInterrupt(void);

	//for light sources with high intensity reading of blue PD should be enabled. Blue photodiode measurement enable.
	//0: 0-10 ms; 1:1-100ms
	void enableColorCompensation(bool colorCompPeriod = 0);
	//adc gain: 0-3: 200, 800, 3200, 25600 fA
	//alsRate 0-7: 1-8 measurements/s
	void setADCGain(uint16_t adcGain);
	void setAmbientLightMeasurementRate(uint8_t alsRate);
	//TODO: set IRED (register #3)
	uint8_t proximityConfig = 0;
private:
	bool colorCompensationEnabled = false;

	uint8_t ambientLightConfig = 0;
	uint8_t interruptConfig = 0;
	float gainFactor;
	uint16_t rawProximity;
	uint16_t rawAmbientLight;

	float getBlueRatio(void);
	uint16_t blue1PD;
	uint16_t blue2PD;
	uint16_t concatResults(uint8_t upperByte, uint8_t lowerByte);
	void writeOut(uint16_t regNum, uint16_t val);
	void enableOnDemandReading(void);
	void enablePeriodicMeasurements(void);
};

#endif		/* PALS2_H_INCLUDED */
