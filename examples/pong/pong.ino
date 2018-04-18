

#include <Pals2.h>
Pals2 pals;
void setup() {
	Serial.begin(9600);
	pals.begin();
}
void loop() {
	Serial.println(pals.getRawProximityOnDemand());
}
