#include <LedString.h>
#define DATA_PIN 2
#define NUM_LEDS 8

LedString myLedString;
CRGB leds[NUM_LEDS];

class ColorSequence : public LedHandler {
  private:
    CRGB* colors;
    int colorCount;
    int nextColor;
  public:
    ColorSequence (char label, uint32_t interval, CRGB colors[], int count) : LedHandler(label, interval) {
      this->colors = colors;
      this->colorCount = count;
      this->nextColor = count;
    }
    void start(LedString ls) {
      if (this->enabled) {
        this->nextColor++;
        if (this->nextColor >= this->colorCount) {
          this->nextColor = 0;
        }
      }
    }
    void loop(LedString ls, int ledNumber) {
      ls.leds[ledNumber] = this->colors[this->nextColor];
    }
};

CRGB colors1[] = { CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black };
CRGB colors2[] = { CRGB::Blue, CRGB::White };

void setup() {
  Serial.begin(115200); while (!Serial) { ; } 
  Serial.println("\n*****");

  FastLED.clear();
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  myLedString.setup(leds);

  Serial.println("Adding custom handlers");
  ColorSequence *cs1 = new ColorSequence('1', 500, colors1, 4);
  myLedString.addHandler(cs1);
  ColorSequence *cs2 = new ColorSequence('2', 500, colors2, 2);
  myLedString.addHandler(cs2);

  Serial.println("Calling begin");
  myLedString.begin("11112222");
}

void loop() {
  myLedString.loop();
}

