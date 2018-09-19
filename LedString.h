
#ifndef LedString_h
#define LedString_h

#include <FastLED.h>

#define DATA_PIN 5

typedef void(*LedStringCustomFunction)(int);
typedef void(*LedStringCycleSetup)();

class LedString
{
public:
  CRGB* leds = 0;
  long lastEventTime = 0L;
  int numSwitches = 0;
  int switchIndex = 0;
  int switchIndexToToggle = 0;
  long nextSwitchTime = 0;

  void doSetup(String pattern);
  void doStart();
  void doLoop();

  bool isOn(int led);
  void turnOn(int led);
  void turnOff(int led);
  void turnAllOn();
  void turnAllOff();

  
  void setCustom(LedStringCustomFunction);   // sets the function to call for behavior "C"
  void setCycleSetup(LedStringCycleSetup);   // sets the function to call at the start of each cycle through the leds

private:
  String _pattern;
  int _length;
  // min/max brightness range of normal and intense flickers
  int FIRE_MIN = 150; int FIRE_MAX = 190; int FLICKER_MIN = 130; int FLICKER_MAX = 225;  ////// WS2811, WS2812, WS2812B, WS2813
//int FIRE_MIN = 80; int FIRE_MAX = 160; int FLICKER_MIN = 10; int FLICKER_MAX = 230;    ////// NEOPIXEL


  int FLICKER_RATE = 80;   // ms between flickers
  int FLICKER_EXTRA = 2;   // when brightness is this close to FIRE_MIN or FIRE_MAX, use FLICKER_MIN or FLICKER_MAX instead
  
  long SWITCH_MIN = 2000L; // min/max ms between toggling a random Switched light
  long SWITCH_MAX = 5000L;

  void flicker(int led);
  void turnLitOn();
  void turnLitOrSwitchedOn();
  bool isEventTime();
  void checkSwitch(int led);
  void setupSwitches();
  LedStringCycleSetup cycleSetup;
  LedStringCustomFunction customHandler;
  void doCycle();
  static void dummyCustom(int led);
  static void dummyCycleSetup();
};

#endif
