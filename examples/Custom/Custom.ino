#include <LedString.h>
#define DATA_PIN 2
#define NUM_LEDS 5

LedString myLedString;
CRGB leds[5];

void sequence(CRGB* leds, int ledNumber, CRGB colors[], int count, uint32_t interval) {
  // step through an array of colors at intervals; 
  // Limitation: each color must occur only once or it will get stuck alternating between 
  // the first occurrence of that color and the next color. 
  static uint32_t lastChange = 0;
  uint32_t timeNow = myLedString.currentTime();
  if (lastChange + interval < timeNow || timeNow < lastChange) {
    for (int i=0; i<count-1; i++) {
      if (leds[ledNumber] == colors[i]) {
        leds[ledNumber] = colors[i+1];
        lastChange = timeNow;
        FastLED.show();
        return;
      }
    }
    leds[ledNumber] = colors[0];
    lastChange = timeNow;
  }
}

void blink1(CRGB* leds, int i) {
  CRGB colors[] = { CRGB::Red, CRGB::White, CRGB::Blue };
  sequence(leds, i, colors, 3, 1000);
}

void blink2(CRGB* leds, int i) {
  CRGB colors[] = { CRGB::Red, CRGB::Green, CRGB::Blue };
  sequence(leds, i, colors, 3, 1000);
}

void setup() {
  FastLED.clear();
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  myLedString.doSetup("12rgb", leds);
  myLedString.setHandler('2', &blink2);
  myLedString.setHandler('1', &blink1);
  myLedString.doStart();
}

void loop() {
  myLedString.doLoop();
}
