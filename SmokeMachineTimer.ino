#define USE_PRINT
// The serial port is ised for DMX transmit normally. If you want debug-print define USE_PRINT.

#ifndef USE_PRINT
#include <DMXSerial.h>
#endif

float smokeOnTime = 0;
float smokeOffTime = 0;
uint8_t smokeLevel = 0;

uint32_t timeSmokeToggled = 0;
bool stateSmoke = true;

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	#ifdef USE_PRINT
	Serial.begin(115200);
	Serial.println("Start SmokeMachineTimer");
	#else
	DMXSerial.init(DMXController);
	#endif
}

void checkPots() {
	int pot1 = analogRead(A1);
	pot1 = map(pot1, 780, 0, 0, 255); // Scale inverted pot 0-3.7V (0-780)
	pot1 = max(pot1, 0); // Avoid negative numbers
	smokeOnTime = (float)pot1 / 255;

	int pot2 = analogRead(A2);
	pot2 = map(pot2, 780, 0, 0, 255); // Scale inverted pot 0-3.7V (0-780)
	pot2 = max(pot2, 0); // Avoid negative numbers
	smokeOffTime = (float)pot2 / 255;

	// Parse multipositionswitch
	smokeLevel = 255;
}

void tick() {
	// Smoke period
	float secondsSinceSmokeToggled = ((float)(millis() - timeSmokeToggled) / 1000);
	float smokeIntervalOn = smokeOnTime*9+1; // 1-10 seconds
	float smokeIntervalOff = smokeOffTime*9+1; // 1-10 seconds
	if (stateSmoke == true && secondsSinceSmokeToggled > smokeIntervalOn) {
		#ifdef USE_PRINT
		Serial.print("Toggle smoke off after");
		Serial.println(smokeIntervalOn);
		#endif
		stateSmoke = !stateSmoke;
		timeSmokeToggled = millis();
	} else if (stateSmoke == false && secondsSinceSmokeToggled > smokeIntervalOff) {
		#ifdef USE_PRINT
		Serial.print("Toggle smoke on after");
		Serial.println(smokeIntervalOff);
		#endif
		stateSmoke = !stateSmoke;
		timeSmokeToggled = millis();
	}

	// Set smoke level out
	if (stateSmoke) {
		digitalWrite(LED_BUILTIN, HIGH);
		#ifndef USE_PRINT
		DMXSerial.write(1, smokeLevel);
		#endif
	} else {
		digitalWrite(LED_BUILTIN, LOW);
		#ifndef USE_PRINT
		DMXSerial.write(1, 0);
		#endif
	}

	#ifdef USE_PRINT
	Serial.print("On time: ");
	Serial.print(smokeIntervalOn);
	Serial.print(" Off time: ");
	Serial.print(smokeIntervalOff);
	Serial.print(" time since toggle: ");
	Serial.println(secondsSinceSmokeToggled);
	#endif
}

void loop() {
	checkPots();
	tick();
	delay(100);
}

