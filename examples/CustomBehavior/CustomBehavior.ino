// Custom behavior example - toggles leds between red and blue every second

#include "LedString.h"
LedString lights;

#define DATA_PIN 3
#define NUM_LEDS 8

// pattern is Red, Green, Blue, White, Custom, Custom, Custom, Custom
String pattern = "RGBWCCCC";

// allocate space for 8 leds
CRGB leds[NUM_LEDS];

uint32_t customInterval = 1000;             // ms between toggle events
uint32_t lastCustomTime = -customInterval;  // make the first event happen immediately

void customBehavior(int led) {
  uint32_t msNow = millis();
  // if customInterval has elapsed since the last toggle, it's time to toggle
  if (msNow - lastCustomTime > customInterval)
  {
    lastCustomTime = msNow;
    // if the led is red make it blue, else make it red
    if (lights.leds[led].red == 255) {
      lights.leds[led] = CRGB::Blue;
    } else {
      lights.leds[led] = CRGB::Red;
    }
  }
  FastLED.show();   // physically update the leds
}

void setup() {
  // addLeds must be called here rather than being done by the library
  // because FastLED requires the pin number to be a compile-time constant.
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  
  lights.doSetup(pattern, leds);
  lights.setCustom(customBehavior);
  lights.doStart();
}

void loop() {
  lights.doLoop();
}
