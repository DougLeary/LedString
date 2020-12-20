#ifndef LedString_h
#define LedString_h

#include <FastLED.h>

typedef void(*LedStringCustomFunction)(CRGB led);
typedef void(*LedStringCycleSetup)();
typedef void(*LedStringAddLeds)(CRGB leds, int length);

class LedString
{
public:
  CRGB* leds = 0;
  long lastEventTime = 0L;
  int numSwitches = 0;
  int switchIndex = 0;
  int switchIndexToToggle = 0;
  long nextSwitchTime = 0;
  String pattern;

  void doSetup(String pattern, CRGB* ledArray);
//  void doSetup(String pattern);
  void doStart();
  void doLoop();
  void setPattern(String st);

  bool isOn(int led);
  void turnOn(int led);
  void turnOff(int led);
  void turnAllOn();
  void turnAllOff();
  void resetAll();
  
  void setCustom(LedStringCustomFunction);   // sets the function to call for behavior "C"
  void setCycleSetup(LedStringCycleSetup);   // sets the function to call at the start of each cycle through the leds

private:
  int _length;
  // min/max brightness range of normal and intense flickers
  int FIRE_MIN = 150; int FIRE_MAX = 190; int FLICKER_MIN = 130; int FLICKER_MAX = 225;  ////// WS28xx
//  int FIRE_MIN = 80; int FIRE_MAX = 160; int FLICKER_MIN = 10; int FLICKER_MAX = 230;    ////// NEOPIXEL


  int FLICKER_RATE = 80;   // ms between brightness changes
  int FLICKER_EXTRA = 2;   // when brightness is this close to FIRE_MIN or MAX, FLICKER_MIN or MAX is used
  
  long AVERAGE_SWITCH_INTERVAL = 20000L; // desired average ms between toggling a switched led
  long MIN_SWITCH_INTERVAL = 5000L;

  void flicker(int led);
  void turnLitOn();
  void turnOnSteadies();
  bool isEventTime();
  void checkSwitch(int led);
  void setupSwitches();
  LedStringCycleSetup cycleSetup;
  LedStringCustomFunction customHandler;
  void doCycle();
  static void dummyCustom(CRGB led);
  static void dummyCycleSetup();
};

#endif
