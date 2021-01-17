// LedString Simple Test - run a string of 8 leds

#include <LedString.h>
LedString lights;

#define DATA_PIN 2
#define NUM_LEDS 8

// Red, Green, Blue and Fire
String pattern = "RRGGBBFF";

// allocate space for 8 leds
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  Serial.println("\n**************");
  // addLeds must be called here rather than being done by the library
  // because FastLED requires the pin number to be a compile-time constant.
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  
  lights.setup(leds, NUM_LEDS);
  lights.begin(pattern);
}

void loop() {
  lights.loop();
}
