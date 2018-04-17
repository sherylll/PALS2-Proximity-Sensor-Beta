#include <Wire.h>
#include<Pals2.h>
Pals2 pals;

void setup() {
	Serial.begin(9600);
	pals.begin();
	pals.setProximityMeasurementRate(128);
}
void loop() {
	Serial.println("On-demand measurement");
	unsigned long start = millis();
	Serial.print("Raw proximity value: ");
	Serial.print(pals.getRawProximityOnDemand());
	Serial.print("; Raw ambient light value: ");
	Serial.print(pals.getRawAmbientLightOnDemand());
	Serial.print("; elapsed time: ");
	Serial.println(millis()-start);

	Serial.println("Periodic measurement");
	start = millis();
	pals.updateData();
	Serial.print("Raw proximity value: ");
	Serial.print(pals.getRawProximity());
	Serial.print("; raw ambient light value: ");
	Serial.print(pals.getRawAmbientLight());
	Serial.print("; elapsed time: ");
	Serial.println(millis()-start);
	delay(1000);
}
