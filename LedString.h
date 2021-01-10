#ifndef LedString_h
#define LedString_h

#include <FastLED.h>

#define MAX_LEDS 100    // max number of behaviors, i.e. leds; need to make this dynamic and depend on the size of the pattern set by the app
#define MAX_LED_STRING_HANDLERS 20
class LedHandler {    // implements led behaviors 
  public:
    char label;                     // 1-char label to use in pattern
    uint32_t interval = 0;          // milliseconds between events; if 0 do once only at startup 
    uint32_t whenLast = 0;          // time of last event
    bool enabled = false;           // true if the handler should execute during this cycle
    LedHandler(char label, uint32_t interval);
    virtual void start();                   // executed before cycling through the leds
    virtual void loop(CRGB *leds, int i);   // executed on each led with this label
};

class SimpleHandler : public LedHandler {
  public:
    CRGB::HTMLColorCode color;
    SimpleHandler(char label, uint32_t interval, CRGB::HTMLColorCode color);
    virtual void loop(CRGB *leds, int i);
};

class LedString
{
public:
  CRGB* leds = 0;
  String pattern;

  // min/max brightness range of normal and intense flickers
  static const int FIRE_MIN = 150; 
  static const int FIRE_MAX = 190; 
  static const int FLICKER_MIN = 130; 
  static const int FLICKER_MAX = 225;  ////// WS28xx
//  int FIRE_MIN = 80; int FIRE_MAX = 160; int FLICKER_MIN = 10; int FLICKER_MAX = 230;    ////// NEOPIXEL

  static const int FLICKER_RATE = 80;  // ms between brightness changes
  static const int FLICKER_EXTRA = 2;  // when brightness is this close to FIRE_MIN or MAX, FLICKER_MIN or MAX is used

  // timing for switched lights
  static const uint32_t AVERAGE_SWITCH_INTERVAL = 20000; // desired average ms between toggling a random switched led
  static const uint32_t MIN_SWITCH_INTERVAL = 5000;      // shortest time between toggling

  void setup(CRGB* ledArray);
  void begin(String pattern);
  void loop();
  uint32_t currentTime();
  uint32_t previousTime();
  void addHandler(LedHandler* h);
  void addSimpleHandler(char label, uint32_t interval, CRGB::HTMLColorCode color);
  void setPattern(String st);
  void countSwitches();

  bool isOn(int led);
  void turnOn(int led);
  void turnOff(int led);
  void turnAllOn();
  void turnAllOff();
  void resetAll();
  void setLed(int i, CRGB::HTMLColorCode color);
  
private:
  int _length;

  void addBuiltInHandlers();
  void addBehavior(char label);
  void populateBehaviors();
  bool isEventTime(uint32_t interval, uint32_t &previousTime);
  void enableHandlers();
  void doBehaviors();
  void doCycle();
};

#endif
