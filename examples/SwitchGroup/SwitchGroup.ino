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
#define NUM_LEDS 8

LedString myLedString;
CRGB leds[NUM_LEDS];

void sayColor(String msg, CRGB color) {
  Serial.print("  " + msg + ": "); Serial.print(color.red);
  Serial.print(", "); Serial.print(color.green);
  Serial.print(", "); Serial.println(color.blue);
}

class SwitchGroup : public LedHandler {
  private:
    int switchCount;
    int countdown;
    CRGB color;
    uint32_t minInterval;
    uint32_t maxInterval;
  public:
    SwitchGroup(char label, uint32_t minInterval, CRGB color, uint32_t maxInterval) : LedHandler(label, 0) {
      this->interval = 0;
      this->minInterval = interval;
      this->maxInterval = maxInterval;
      this->color = color;
      sayColor("color", color);
      sayColor("this->color", this->color);
    }
    
    void setup() {
      // count the switched leds in this group
      switchCount = 0;
      countdown = 0;
      int len = myLedString.pattern.length();
      for (int i = 0; i < len; i++) {
        if (myLedString.pattern.charAt(i) == this->label) {
          switchCount++;
        }
      }
    }

    void loop(CRGB* leds, int ledNumber) {
      // toggle a led when the countdown reaches 0
      if (this->countdown > 0) {
        countdown--;
      } else {
        if (leds[ledNumber] == color) {
          Serial.print(" led "); Serial.print(ledNumber); Serial.println(" black");
          leds[ledNumber] = CRGB::Black;
        } else {
          Serial.print(" led "); Serial.print(ledNumber);
          sayColor(" toggle", this->color);
          leds[ledNumber] = this->color;
        }
        // set random time and led for next toggle
        this->enabled = false;    // once we've toggled, disable until the next cycle
        this->countdown = random(switchCount);
        this->interval = random(minInterval, maxInterval);
        Serial.print("New countdown "); Serial.println(this->countdown); 
      }      
    }
};

void setup() {
  Serial.begin(115200); while (!Serial) { ; } 
  Serial.println("\n*****");

  FastLED.clear();
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  myLedString.setup(leds);

  Serial.println("Adding custom handlers");
  SwitchGroup *sg1 = new SwitchGroup('1', 1000, CRGB::Green, 8000);
  myLedString.addHandler(sg1);
  SwitchGroup *sg2 = new SwitchGroup('2', 1000, CRGB::White, 4000);
  myLedString.addHandler(sg2);

  Serial.println("Calling begin");
  myLedString.begin("11111222");
}

void loop() {
  myLedString.loop();
}

