// LedString ActiveGroup Test
// Uses an ActiveGroup to simulate habitation activity in buildings

#include <LedString.h>
LedString lights;

#define DATA_PIN 2
#define NUM_LEDS 20

String pattern = "AAAAAAAAAAAAAAAAAAAA";

// allocate space for leds
CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  lights.setup(leds, NUM_LEDS);
  lights.begin(pattern);
}

void loop() {
  lights.loop();
}
