#include "LedString.h";

LedString lights;

void setup() {
  lights.doSetup("llllsssf llllsssf llllsssf llllsssf ll");  // test string of 50 leds
  delay(1000);
  lights.doStart();
}

void loop() {
  // put your main code here, to run repeatedly:
  lights.doLoop();
}
