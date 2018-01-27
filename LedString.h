
#ifndef LedString_h
#define LedString_h

#include "FastLED.h"

// detect ESP8266, otherwise assume Arduino
#if defined(ESP8266)
#define DATA_PIN D3
#else
#define DATA_PIN 3
#endif

typedef void (*LedStringCustomFunction)(int);

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

  void setCustom(LedStringCustomFunction);   // accepts a function to be called for Custom behavior

private:
  String _pattern;
  int _length;
  int FIRE_MIN = 150;      // min/max brightness range of normally flickering fire
  int FIRE_MAX = 190;

  int FLICKER_MIN = 130;   // min/max brightness of occasional extra-intense flickers
  int FLICKER_MAX = 225;

  int FLICKER_RATE = 80;   // ms between flickers
  int FLICKER_EXTRA = 2;   // when flicker brightness is this close to FIRE_MIN or FIRE_MAX, make it extra intense
  
  long SWITCH_MIN = 2000L; // min/max ms between toggling a random Switched light
  long SWITCH_MAX = 5000L;

  void flicker(int led);
  void turnLitOn();
  void turnLitOrSwitchedOn();
  bool isEventTime();
  void checkSwitch(int led);
  void setupSwitches();
  LedStringCustomFunction doCustom;
};

#endif
