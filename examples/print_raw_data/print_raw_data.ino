/**
 * PALS-2 reading proximity and ambient light raw value example.
 * Author: Yuxi Sun
 * Measurements can be carried out on-demand in order to save energy. However an on-demand measurement is
 * also much slower than an average periodic measurement.
 * Note: enablePeriodicMeasurements() is called to undo the on-demand settings; normally it's not necessary
 * to always be called before updateData()
 */

#include <Wire.h>
#include <Pals2.h>
Pals2 pals;
void setup() {
	Serial.begin(9600);
	pals.begin();
}
void loop() {
	unsigned long start;
	Serial.println("On-demand measurement");
	start = millis();
	Serial.print("Raw proximity value: ");
	Serial.print(pals.getRawProximityOnDemand());
	Serial.print("; Raw ambient light value: ");
	Serial.print(pals.getRawAmbientLightOnDemand());
	Serial.print("; elapsed time: ");
	Serial.println(millis()-start);

	Serial.println("Periodic measurement");
	start = millis();
	pals.enablePeriodicMeasurements();
	pals.updateData();
	Serial.print("Raw proximity value: ");
	Serial.print(pals.getRawProximity());
	Serial.print("; raw ambient light value: ");
	Serial.print(pals.getRawAmbientLight());
	Serial.print("; elapsed time: ");
	Serial.println(millis()-start);
	delay(1000);
}
