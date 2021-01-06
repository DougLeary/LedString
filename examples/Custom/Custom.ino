#include <LedString.h>
#define DATA_PIN 2
#define NUM_LEDS 5

LedString myLedString;
CRGB leds[5];

void sequence(CRGB* leds, int ledNumber, CRGB colors[], int count) {
  // step through an array of colors at intervals; 
  // Limitation: each color must occur only once or it will get stuck alternating between 
  // the first occurrence of that color and the next color. 
  for (int i=0; i<count-1; i++) {
    if (leds[ledNumber] == colors[i]) {
      leds[ledNumber] = colors[i+1];
      Serial.print(" color ");
      Serial.print(i+1);
      return;
    }
  }
  leds[ledNumber] = colors[0];
  Serial.print(" color ");
  Serial.print(0);
}

void blink1(CRGB* leds, int i) {
  CRGB colors[] = { CRGB::White, CRGB::Black };
  sequence(leds, i, colors, 2);
}


void blink2(CRGB* leds, int i) {
  CRGB colors[] = { CRGB::Red, CRGB::Green, CRGB::Blue };
  sequence(leds, i, colors, 3);
}

void setup() {
  Serial.begin(115200); while (!Serial) { ; } 
  Serial.println("\n*****");

  FastLED.clear();
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  myLedString.setup(leds);

  Serial.println("Adding custom handlers");
  myLedString.addHandler('2', &blink2, 3000);
  myLedString.addHandler('1', &blink1, 2000);
  Serial.println("Calling begin");
  myLedString.begin("21RGB");
}

void loop() {
  myLedString.loop();
}
