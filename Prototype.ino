#include "FastLED.h"

// detect ESP8266, otherwise assume Arduino
#if defined(ESP8266)
#define DATA_PIN D3
#else
#define DATA_PIN 3
#endif

class LedString
{
  private:
    // dummy pointer to the leds array to satisfy the compiler; actual array will be redefined at runtime when we know the size
    CRGB* leds = 0;

    String _pattern;
    int _length;
    int FIRE_MIN = 150;      // min/max brightness range of normally flickering fire
    int FIRE_MAX = 190;
    int FLICKER_MIN = 130;   // min/max brightness of occasional extra-intense flickers
    int FLICKER_MAX = 225;
    int FLICKER_RATE = 80;   // ms between flickers
    int FLICKER_EXTRA = 2;   // when brightness is this close to FIRE_MIN or FIRE_MAX, a more intense flicker occurs
    long SWITCH_MIN = 2000L;   // min/max ms between toggling a Switched light on or off
    long SWITCH_MAX = 5000L;

  public:
    long lastEventTime = 0L;
    int numSwitches = 0;
    int switchIndex = 0;
    int switchIndexToToggle = 0;
    long nextSwitchTime = 0;

    LedString() {
    }

    void begin(String pattern) {
      _pattern = pattern;
      _pattern.replace(" ", "");
      _pattern.toUpperCase();
      _length = _pattern.length();
      leds = (CRGB*) malloc(_length * sizeof(CRGB));
      FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, _length);
      setupSwitches();
    }

    void flicker(int led) {
      int value = random(FIRE_MIN, FIRE_MAX);
      // occasional intense flicker
      if (value <= FIRE_MIN + FLICKER_EXTRA) {
        value = FIRE_MIN - ((FIRE_MIN - FLICKER_MIN) / (value - FIRE_MIN));
      } else if (value >= FIRE_MAX - FLICKER_EXTRA) {
        value = FIRE_MAX + ((FLICKER_MAX - FIRE_MAX) / (FIRE_MAX - value));
      }
      leds[led] = CHSV(25, 187, value);
      FastLED.show();
    }

    // custom code for specific leds
    long interval = 1500L;
    long lastCustom = -interval;

    void doCustom() {
      // alternate two leds red and blue every 2 seconds
      long msNow = millis();

      if (msNow - lastCustom >= interval) {
        if (leds[0].red == 255) {
          leds[0] = CRGB::Blue;
          leds[7] = CRGB::Red;
        }
        else {
          leds[0] = CRGB::Red;
          leds[7] = CRGB::Blue;
        }
        FastLED.show();
        lastCustom = msNow;
      }
    }

    // these are encapsulated as functions in case we want to use HSV values instead
    bool isOn(int led) {
      return (
               (leds[led].red == 255) &&
               (leds[led].green == 255) &&
               (leds[led].blue == 255));
    }

    void turnOn(int led) {
      leds[led] = CRGB::White;
      FastLED.show();
    }

    void turnOff(int led) {
      leds[led] = CRGB::Black;
      FastLED.show();
    }

    void turnAllOn() {
      Serial.println("all on");
      for (int i = 0; i < _length; i++) {
        leds[i] = CRGB::White;
        Serial.print(leds[i].red);
        Serial.print(",");
        Serial.print(leds[i].green);
        Serial.print(",");
        Serial.println(leds[i].blue);
      }
      FastLED.show();
    }

    void turnAllOff() {
      Serial.begin(9600);
      delay(100);
      Serial.println(_length);
      for (int i = 0; i < _length; i++) {
        leds[i] = CRGB::Black;
        Serial.print(leds[i].red);
        Serial.print(",");
        Serial.print(leds[i].green);
        Serial.print(",");
        Serial.println(leds[i].blue);
      }
      FastLED.show();

    }

    void turnLitOn() {
      // turn on all "Lit" leds
      for (int i = 0; i < _length; i++) {
        if (_pattern.charAt(i) == 'L') {
          leds[i] = CRGB::White;
        }
      }
      FastLED.show();
    }

    void turnLitOrSwitchedOn() {
      // turn on all "Lit" or "Switched" leds
      for (int i = 0; i < _length; i++) {
        char type = _pattern[i];
        if ((type == 'L') || (type == 'S')) {
          leds[i] = CRGB::White;
        }
      }
      FastLED.show();
    }

    bool isEventTime() {
      long msNow = millis();
      long gap = msNow - lastEventTime;
      if (gap < 0) {
        gap = FLICKER_RATE;
      }
      if (gap >= FLICKER_RATE) {
        lastEventTime = msNow;
        return true;
      } else {
        return false;
      }
    }

    void checkSwitch(int led) {
      // increment switchIndex until it reaches switchIndexToToggle, then toggle that led on/off
      if (switchIndex < switchIndexToToggle) {
        switchIndex++;
      }
      else {
        if (isOn(led)) {
          turnOff(led);
        }
        else {
          turnOn(led);
        }
        switchIndexToToggle = random(0, numSwitches);
        nextSwitchTime = millis() + random(SWITCH_MIN, SWITCH_MAX);
      }
    }

    void setupSwitches() {
      for (int i = 0; i < _length; i++) {
        if (_pattern.charAt(i) == 'S') {
          numSwitches++;
        }
      }
      switchIndexToToggle = random(0, numSwitches);
      nextSwitchTime = millis();
    }

    void start() {
      turnLitOrSwitchedOn();
      long msNow = millis();
      for (int i = 0; i < _length; i++) {
        char ch = _pattern.charAt(i);
        switch (ch) {
          case 'S':
            if (msNow >= nextSwitchTime) checkSwitch(i);
            break;
          case 'F':
            flicker(i);
            break;
        }
      }
      doCustom();
    }

    void loop() {
      if (isEventTime()) {
        long msNow = millis();
        int switchIndex = 0;

        for (int i = 0; i < _length; i++) {
          char ch = _pattern.charAt(i);
          switch (ch) {
            case 'S':
              if (msNow >= nextSwitchTime) checkSwitch(i);
              break;
            case 'F':
              flicker(i);
              break;
          }
        }
        doCustom();
      }
    }

};

// use one letter for each LED; blanks are ignored and can be inserted for clarity
LedString lights;

void setup() {
  lights.begin("cfffsssc");
  lights.turnAllOff();
  delay(2000);
  lights.start();
}

void loop() {
  lights.loop();
}

