// Custom ActiveGroup
// This sketch demonstrates how to customize the ActiveGroup behavior.
// To do this you simply create an instance of ActiveGroup with non-default parameters.

#include <LedString.h>
LedString lights;

#define DATA_PIN 2
#define NUM_LEDS 10
CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  lights.setup(leds, NUM_LEDS);

  // Turn leds on and off every quarter second, keeping about 20% of the leds on at once
  ActiveGroup *group = new ActiveGroup('X', 250, 250, CRGB::Green, 20);
  lights.addHandler(group);
  lights.begin("XXXXXXXXXX");
}

void loop() {
  lights.loop();
}
