/**
 * @file Pals2.cpp
 * @brief Arduino library to control the proximity and ambient light sensor PALS2 from Infineon (packaged by Vishay as VCNL4135X01)
 * @author Yuxi Sun
 * @bug no Blue-PD value updates -> getIlluminance() not working;
 * @bug	in register 83h sensor measurement freezes if IRED output is not default(0): due to circuitry?
 */

#include "Pals2.h"
#include <math.h>

Pals2::Pals2() {
}

void Pals2::begin(void) {
	Wire.begin();
	//reset sensor
	resetSensor();
	//enables periodic measurement (stand-by)
	enablePeriodicMeasurements();
}

void Pals2::updateData(void) {
	uint16_t measurements[NUM_MEASUREMENTS_BLUE];
	Wire.beginTransmission(SLAVE_ADDRESS);
	uint8_t i = 0;
	//Measurement values start at 0x86
	if (!colorCompensationEnabled)
		Wire.requestFrom(SLAVE_ADDRESS, NUM_MEASUREMENTS, PROXIMITY_HIGH_BYTE,
		PALS2_REG_SIZE, 0);
	else
		Wire.requestFrom(SLAVE_ADDRESS, NUM_MEASUREMENTS_BLUE,
		PROXIMITY_HIGH_BYTE, PALS2_REG_SIZE, 0);
	while (Wire.available()) {
		measurements[i] = Wire.read();
		i++;
	}
	Wire.endTransmission();

	rawProximity = concatResults(measurements[0], measurements[1]);
	rawAmbientLight = concatResults(measurements[2], measurements[3]);
	if (colorCompensationEnabled) {
		blue1PD = concatResults(measurements[6], measurements[7]);
		blue2PD = concatResults(measurements[8], measurements[9]);
	}
}

float Pals2::getIlluminance(void) {
	return rawAmbientLight / 100; //according to Fig. 9, how to make of the formulas on Page 6?
}

uint16_t Pals2::getRawProximity(void) {
	return rawProximity;
}

uint16_t Pals2::getRawAmbientLight(void) {
	return rawAmbientLight;
}

void Pals2::resetSensor(void) {
	writeOut(PROXIMITY_CONFIG, 0x00);
	writeOut(IRED_CONFIG, 0x00);
	writeOut(ALS_CONFIG, 0x00);
	writeOut(ALS_COMPENSATION, 0x00);
	writeOut(INTERRUPT_CONFIG, 0x00);
}

void Pals2::enablePeriodicMeasurements(void) {
	writeOut(COMMAND_REGISTER, 0x07);
}

void Pals2::enableProximityOffsetCompensation(void) {
	proximityConfig |= 0x08;
	writeOut(PROXIMITY_CONFIG, proximityConfig);
}

void Pals2::disableProximityOffsetCompensation(void) {
	proximityConfig &= ~0x08;
	writeOut(PROXIMITY_CONFIG, proximityConfig);
}

void Pals2::setProximityMeasurementRate(uint16_t rate) {
	if (rate > 256)
		rate = 256;
	//clear the bits for measurement rate
	proximityConfig &= ~0x07;
	//prx_rate = ld(rate) - 1
	uint8_t prx_rate = -1;
	while (rate >>= 1)
		prx_rate++;
	proximityConfig |= prx_rate;
	writeOut(PROXIMITY_CONFIG, proximityConfig);
}

void Pals2::setInterruptPersistence(uint8_t persistence) {
	uint8_t counts = persistence;
	if (persistence > 128)
		counts = 128;
	//clear the highest 3 bits
	interruptConfig &= ~(0x07 << 5);
	//find the highest set bit
	uint8_t i = 0;
	while (counts >>= 1)
		i++;
	//set the highest 3 bits
	interruptConfig |= (i << 5);
	writeOut(INTERRUPT_CONFIG, interruptConfig);
}

void Pals2::enableProximityInterrupt(uint16_t topThreshold,
		uint16_t bottomThreshold) {
	//interrupt on exceeding both top & bottom thresholds
	interruptConfig |= (0x03 << 3);
	writeOut(INTERRUPT_CONFIG, interruptConfig);
	writeOut(PROX_INT_BOT_HB, bottomThreshold & (0xFF << 8));
	writeOut(PROX_INT_BOT_LB, bottomThreshold & 0xFF);
	writeOut(PROX_INT_TOP_HB, topThreshold & (0xFF << 8));
	writeOut(PROX_INT_TOP_LB, topThreshold & 0xFF);
}

void Pals2::disableProximityInterrupt(void) {
	//disables both top & bottom thresholds
	interruptConfig &= ~(0x03 << 3);
	writeOut(INTERRUPT_CONFIG, interruptConfig);
}

void Pals2::enableAmbientLightInterrupt(uint16_t topThreshold,
		uint16_t bottomThreshold) {
	//interrupt on exceeding both top & bottom thresholds
	interruptConfig |= (0x03 << 1);
	writeOut(INTERRUPT_CONFIG, interruptConfig);

	writeOut(ALS_INT_BOT_HB, bottomThreshold & (0xFF << 8));
	writeOut(ALS_INT_BOT_LB, bottomThreshold & 0xFF);
	writeOut(ALS_INT_TOP_HB, topThreshold & (0xFF << 8));
	writeOut(ALS_INT_TOP_LB, topThreshold & 0xFF);
}

void Pals2::disableAmbientLightInterrupt(void) {
	interruptConfig |= (0x03 << 1);
	writeOut(INTERRUPT_CONFIG, interruptConfig);
}

void Pals2::enableColorCompensation(bool colorCompPeriod) {
	writeOut(ALS_COMPENSATION, 0x01 + 0x02 * colorCompPeriod);
	colorCompensationEnabled = true;
}

float Pals2::getBlueRatio(void) {
	//TODO: check why always zero
	return (blue1PD - blue2PD) / blue1PD;
}

uint16_t Pals2::concatResults(uint8_t upperByte, uint8_t lowerByte) {
	uint16_t value = 0x0000;
	value = (uint16_t) upperByte << 8;
	value |= (uint16_t) lowerByte;
	return value;
}

void Pals2::setADCGain(uint16_t adcGain) {
	//clears bits 3,4
	ambientLightConfig &= ~(0x03 << 3);
	//default ADC gain is 200 fA (bits 3, 4 = 0)
	switch (adcGain) {
	case 800:
		gainFactor = 22.17;
		ambientLightConfig |= 0x01 << 3;
		break;
	case 3200:
		gainFactor = 6.02;
		ambientLightConfig |= 0x10 << 3;
		break;
	case 25600:
		gainFactor = 0.75;
		ambientLightConfig |= 0x11 << 3;
		break;
	default:
		gainFactor = 81.79;
		break;
	}
	writeOut(ALS_CONFIG, ambientLightConfig);
}

void Pals2::setAmbientLightMeasurementRate(uint8_t alsRate) {
	//max. rate is 8 measurements/s
	if (alsRate > 8)
		alsRate = 8;
	ambientLightConfig &= ~0x07;
	ambientLightConfig |= (alsRate - 1);
	writeOut(ALS_CONFIG, ambientLightConfig);
}

uint16_t Pals2::getRawProximityOnDemand(void) {
	enableOnDemandReading();

	Wire.beginTransmission(SLAVE_ADDRESS);
	Wire.requestFrom(SLAVE_ADDRESS, 2, PROXIMITY_HIGH_BYTE,
	PALS2_REG_SIZE, 0);

	uint8_t upperByte = Wire.read();
	uint8_t lowerByte = Wire.read();
	Wire.endTransmission();

	return concatResults(upperByte, lowerByte);
}

uint16_t Pals2::getRawAmbientLightOnDemand(void) {
	enableOnDemandReading();

	Wire.beginTransmission(SLAVE_ADDRESS);
	Wire.requestFrom(SLAVE_ADDRESS, 2, ALS_HIGH_BYTE,
	PALS2_REG_SIZE, 0);

	uint8_t upperByte = Wire.read();
	uint8_t lowerByte = Wire.read();
	Wire.endTransmission();

	return concatResults(upperByte, lowerByte);
}

void Pals2::enableOnDemandReading(void) {
	//set als_od, prox_od and standby_en
	writeOut(COMMAND_REGISTER, 0x19);
	bool ready = false;
	unsigned long start = millis();
	//busy waiting
	while (!ready) {
		Wire.beginTransmission(SLAVE_ADDRESS);
		Wire.requestFrom(SLAVE_ADDRESS, 1, COMMAND_REGISTER,
		PALS2_REG_SIZE, 0);
		ready = (Wire.read() & 0x40) >> 6;
		Wire.endTransmission();

		//set time out in case sensor gets stuck
		if (millis() - start > 1000)
			break;
	}
}

void Pals2::writeOut(uint16_t regNum, uint16_t val) {
	Wire.beginTransmission(SLAVE_ADDRESS);
	Wire.write(regNum);
	Wire.write(val);
	Wire.endTransmission();
}
