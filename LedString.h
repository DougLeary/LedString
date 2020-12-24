#ifndef LedString_h
#define LedString_h

#include <FastLED.h>

#define MAX_LEDS 100    // max number of behaviors, i.e. leds; need to make this dynamic and depend on the size of the pattern set by the app

typedef void(*LedStringHandler)(CRGB*, int);
typedef void(*LedStringCycleSetup)();

#define MAX_LED_STRING_HANDLERS 20
class LedString
{
public:
  CRGB* leds = 0;
  String pattern;
  long lastEventTime;

  // min/max brightness range of normal and intense flickers
  static const int FIRE_MIN = 150; 
  static const int FIRE_MAX = 190; 
  static const int FLICKER_MIN = 130; 
  static const int FLICKER_MAX = 225;  ////// WS28xx
//  int FIRE_MIN = 80; int FIRE_MAX = 160; int FLICKER_MIN = 10; int FLICKER_MAX = 230;    ////// NEOPIXEL

  static const int FLICKER_RATE = 80;  // ms between brightness changes
  static const int FLICKER_EXTRA = 2;  // when brightness is this close to FIRE_MIN or MAX, FLICKER_MIN or MAX is used

  // timing for switched lights
  static const long AVERAGE_SWITCH_INTERVAL = 20000L; // desired average ms between toggling a random switched led
  static const long MIN_SWITCH_INTERVAL = 5000L;      // shortest time between toggling

  void doSetup(String pattern, CRGB* ledArray);
//  void doSetup(String pattern);
  void doStart();
  void doLoop();
  void setHandler(char, LedStringHandler);
  void setPattern(String st);

  bool isOn(int led);
  void turnOn(int led);
  void turnOff(int led);
  void turnAllOn();
  void turnAllOff();
  void resetAll();
  void setLed(int i, CRGB::HTMLColorCode color);
  void setCycleSetup(LedStringCycleSetup);   // sets the function to call at the start of each cycle through the leds

private:
  int _length;

  // labels is an array of characters, each attached to a handler function that applies a behavior to a led
  // setHandler adds labels and their handlers to these lists
  int handler_count = 0;
  char labels[MAX_LED_STRING_HANDLERS] = "\0";
  LedStringHandler handlers[MAX_LED_STRING_HANDLERS];

  // behaviors is an array of handler pointers, one for each led, executed in sequence by doCycle; 
  // this array is essentially the program for the led string
  int behavior_count = 0;
  LedStringHandler behaviors[MAX_LEDS];

  static void dummyCycleSetup();
  LedStringCycleSetup cycleSetup;

  void setupStandardHandlers();
  void addBehavior(char label);
  void parsePattern(String pattern);
  void turnLitOn();
  void turnOnSteadies();
  bool isEventTime();
  void setupSwitches();
  void doCycle();
};

#endif
