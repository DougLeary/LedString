/*
  SwitchGroup

  Demonstrates a LedString Handler subclass that toggles a random led in a group
  on and off at random times, to simulate habitation in a model building or town.
  For each group you create a new instance of SwitchGroup, assign it a label for
  the pattern string, an ON color for the led, and min/max interval times.
  
  The handler needs only setup() and loop(). In setup() we count the leds that have
  its label and pick a random interval and random led. In loop() we decrement a
  countdown; when it reaches 0 we toggle the current led, disable the handler so
  only this led is toggled, then pick a new random interval and led.
*/

#include <LedString.h>
#define DATA_PIN 2
#define NUM_LEDS 10

LedString myLedString;
CRGB leds[NUM_LEDS];

//void sayColor(String msg, CRGB color) {
//  Serial.print("  " + msg + ": "); Serial.print(color.red);
//  Serial.print(", "); Serial.print(color.green);
//  Serial.print(", "); Serial.println(color.blue);
//}

void setup() {
  Serial.begin(115200); while (!Serial) { ; } 
  Serial.println("\n*****");

  FastLED.clear();
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  myLedString.setup(leds);

  Serial.println("Adding custom handlers");
  SwitchGroup *group1 = new SwitchGroup('1', 1000, 8000, CRGB::Green);
  myLedString.addHandler(group1);
  SwitchGroup *group2 = new SwitchGroup('2', 200, 200, CRGB::White);
  myLedString.addHandler(group2);


  Serial.println("Calling begin");
  myLedString.begin("1111122222");
}

void loop() {
  myLedString.loop();
}

