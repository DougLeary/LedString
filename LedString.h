
#ifndef LedString_h
#define LedString_h

// detect ESP8266, otherwise assume Arduino
#if defined(ESP8266)
#define DATA_PIN D3
#else
#define DATA_PIN 3
#endif

class LedString
{
public:
  long lastEventTime = 0L;
  int numSwitches = 0;
  int switchIndex = 0;
  int switchIndexToToggle = 0;
  long nextSwitchTime = 0;

  void doSetup(String pattern);
  void doStart();
  void doLoop();

private:
  // dummy pointer to the leds array to satisfy the compiler; actual array will be redefined at runtime when we know the size

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
  void doCustom();
  bool isOn(int led);
  void turnOn(int led);
  void turnOff(int led);
  void turnAllOn();
  void turnAllOff();
  void turnLitOn();
  void turnLitOrSwitchedOn();
  bool isEventTime();
  void checkSwitch(int led);
  void setupSwitches();
};

#endif
