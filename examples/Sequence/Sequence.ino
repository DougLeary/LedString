#include <LedString.h>
#define DATA_PIN 2
#define NUM_LEDS 3

LedString myLedString;
CRGB leds[NUM_LEDS];

int loopLimit = 600;
int loopCount = 0;


class ColorSequence : public LedHandler {
  private:
    CRGB::HTMLColorCode* colors;
    int colorCount;
    int nextColor;
  public:
    ColorSequence (char label, uint32_t interval, CRGB::HTMLColorCode colors[], int count) : LedHandler(label, interval) {
      this->colors = colors;
      this->colorCount = count;
      this->nextColor = count;
    }
    void start() {
      if (this->enabled) {
        this->nextColor++;
        if (this->nextColor >= this->colorCount) {
          this->nextColor = 0;
        }
      }
    }
    void loop(CRGB* leds, int ledNumber) {
      loopCount++;
      leds[ledNumber] = this->colors[this->nextColor];
    }
};

CRGB::HTMLColorCode colors1[] = { CRGB::Blue, CRGB::Black, CRGB::Green, CRGB::Black };
CRGB::HTMLColorCode colors2[] = { CRGB::Red, CRGB::Green, CRGB::Blue };

void setup() {
  Serial.begin(115200); while (!Serial) { ; } 
  Serial.println("\n*****");

  FastLED.clear();
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  myLedString.setup(leds);

  Serial.println("Adding custom handlers");
  ColorSequence *cs1 = new ColorSequence('1', 500, colors1, 4);
  myLedString.addHandler(cs1);
  ColorSequence *cs2 = new ColorSequence('2', 500, colors2, 3);
  myLedString.addHandler(cs2);

  Serial.println("Calling begin");
  myLedString.begin("1GB");
}

void loop() {
  if (loopCount < loopLimit) {
  myLedString.loop();
  }
}
