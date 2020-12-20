// LedString Simple Test - run a string of 8 leds

#include <LedString.h>
LedString lights;

#define DATA_PIN 3
#define NUM_LEDS 8

// pattern is Red, Green, Blue, White, Yellow, Fire, Switched, Switched
String pattern = "RGBYWFSS";

// allocate space for 8 leds
CRGB leds[NUM_LEDS];

void setup() {
  // addLeds must be called here rather than being done by the library
  // because FastLED requires the pin number to be a compile-time constant.
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  
  lights.doSetup(pattern, leds);
  lights.doStart();
}

void loop() {
  lights.doLoop();
}
