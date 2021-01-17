#ifndef LedString_h
#define LedString_h

#include <FastLED.h>

#define MAX_LEDS 100    // max number of behaviors, i.e. leds; need to make this dynamic and depend on the size of the pattern set by the app
#define MAX_LED_STRING_HANDLERS 20

class LedString;      // needed because handlers and LedString mutually reference each other.

class LedHandler {    // implements led behaviors 
  public:
    char label;                     // 1-char label to use in pattern
    uint32_t interval = 0;          // milliseconds between events; if 0 do once only at startup 
    uint32_t whenLast = 0;          // time of last event
    bool enabled = false;           // true if the handler should execute during this cycle
    LedHandler(char label, uint32_t interval);
    virtual void setup(LedString ls);       // executed when a pattern is created or changed
    virtual void start(LedString ls);       // executed before cycling through the leds
    virtual void loop(LedString ls, int i); // executed on each led with this label
};

class SimpleHandler : public LedHandler {
  public:
    CRGB color;
    SimpleHandler(char label, uint32_t interval, CRGB color);
    virtual void loop(LedString ls, int i);
};

class FlameHandler : public LedHandler {
  public:
    FlameHandler(char label, uint32_t interval);
    virtual void loop(LedString ls, int i);
};

class ActiveGroup : public LedHandler {
  public:
    int groupCount;
    int groupCountOn;
    int countdown;
    int percentOn;
    bool isTurningOn;
    CRGB color;
    uint32_t minInterval;
    uint32_t maxInterval;

//  public:
    ActiveGroup(char label, uint32_t minInterval, uint32_t maxInterval, CRGB color, int percentOn);
    virtual void setup(LedString ls);
    virtual void start(LedString ls);
    virtual void loop(LedString ls, int i);
};

class LedString
{
public:
  CRGB* leds = 0;
  String pattern;

  static const int FLICKER_RATE = 80; // ms between brightness changes
  static const int FLICKER_EXTRA = 2; // when brightness is this close to FIRE_MIN or MAX, FLICKER_MIN or MAX is used
  static const int FIRE_MIN = 150;
  static const int FIRE_MAX = 190; 
  static const int FLICKER_MIN = 130; 
  static const int FLICKER_MAX = 225;  ////// WS28xx
//  int FIRE_MIN = 80; int FIRE_MAX = 160; int FLICKER_MIN = 10; int FLICKER_MAX = 230;    ////// NEOPIXEL

  const int HABITATION_MIN_INTERVAL = 2000;     // default interval range for built-in ActiveGroup
  const int HABITATION_MAX_INTERVAL = 10000;
  const int HABITATION_DEFAULT_PERCENT = 75;    // default percentage of leds that should be on at any time

  int ledCount;

  void setup(CRGB *ledArray, int ledCount);
  void begin(String pattern);
  void loop();
  uint32_t currentTime();
  uint32_t previousTime();
  void addHandler(LedHandler* h);
  void addSimpleHandler(char label, uint32_t interval, CRGB color);
  void setPattern(String st);

  bool isOn(int led);
  void turnOn(int led);
  void turnOff(int led);
  void turnAllOn();
  void turnAllOff();
  void setLed(int i, CRGB color);
  
private:
  void addBuiltInHandlers();
  void addBehavior(char label, int ledNumber);
  void populateBehaviors();
  bool isEventTime(uint32_t interval, uint32_t &previousTime);
  void setupHandlers();
  void enableHandlers();
  void doBehaviors();
  void doCycle();
};

#endif
