// Custom behavior example - toggle between red and blue every second

#include "LedString.h"
LedString lights;

long customInterval = 1000L;            // set ms between toggle events
long lastCustomTime = -customInterval;  // make the first toggle happen immediately

void customBehavior(int led) {
  long msNow = millis();
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
  lights.doSetup("LCOOOOOO");
  lights.setCustom(customBehavior);
  lights.turnAllOff();
  delay(1000);
  lights.doStart();
}

void loop() {
  lights.doLoop();
}
