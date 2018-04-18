/**
 * PALS-2 reading proximity and ambient light raw value example.
 * Author: Yuxi Sun
 * In this example measurements are be carried out on-demand. Raw proximity and ambient light
 * values are printed.
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
}
