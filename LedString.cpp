/*
LedString class by Doug Leary 2018
Requires FastLED library
Tested Controllers:
Arduino Uno
NodeMCU 1.0 (ESP-12E)
Tested LED devices:
WS2811
NEOPIXEL ring
*/

#include "FastLED.h"
#include "LedString.h"

uint32_t _time = 0;
uint32_t _lastEventTime = 0;

uint32_t LedString::currentTime() {
  return _time;
}

uint32_t LedString::lastEventTime() {
  return _lastEventTime;
}

void LedString::setHandler(char label, LedStringHandler f) {
  for (int i=0; i<handler_count; i++) {
    if (labels[i] == label) {
      // replace handler
      handlers[i] = f;
      return;
    }
  }
  // new label; add handler
  labels[handler_count] = label;
  handlers[handler_count] = f;
  handler_count++;
}

void LedString::setLed(int i, CRGB::HTMLColorCode color) {
  leds[i] = color;
}

////// standard handlers
void setRed(CRGB* leds, int i) {
  leds[i] = CRGB::Red;
}

void setGreen(CRGB* leds, int i) {
  leds[i] = CRGB::Green;
}

void setBlue(CRGB* leds, int i) {
  leds[i] = CRGB::Blue;
}

void setYellow(CRGB* leds, int i) {
  leds[i] = CRGB::Yellow;
}

void setWhite(CRGB* leds, int i) {
  leds[i] = CRGB::White;
}

void setBlack(CRGB* leds, int i) {
  leds[i] = CRGB::Black;
}

void flicker(CRGB* leds, int led) {
  int value = random(LedString::FIRE_MIN, LedString::FIRE_MAX);
  // occasional intense flicker
  if (value <= LedString::FIRE_MIN + LedString::FLICKER_EXTRA) {
    value = LedString::FIRE_MIN - ((LedString::FIRE_MIN - LedString::FLICKER_MIN) / (value - LedString::FIRE_MIN + 1));
  }
  else if (value >= LedString::FIRE_MAX - LedString::FLICKER_EXTRA) {
    value = LedString::FIRE_MAX + ((LedString::FLICKER_MAX - LedString::FIRE_MAX) / (LedString::FIRE_MAX - value));
  }
  leds[led] = CHSV(25, 187, value);
}


//////

void LedString::resetAll() {
  FastLED.clear();
}

void LedString::setCycleSetup(LedStringCycleSetup f) {
  cycleSetup = f;
}

bool LedString::isOn(int led) {
  return (
    (leds[led].red == 255) &&
    (leds[led].green == 255) &&
    (leds[led].blue == 255));
}

void LedString::turnOn(int led) {
  setWhite(leds, led);
}

void LedString::turnOff(int led) {
  setBlack(leds, led);
}

void LedString::turnAllOn() {
  for (int i = 0; i < _length; i++) {
    setWhite(leds, i);
  }
}

void LedString::turnAllOff() {
  for (int i = 0; i < _length; i++) {
    setBlack(leds, i);
  }
}

bool LedString::isEventTime(uint32_t &previousTime) {
  // time sync function; everything else is based on _time and _lastEventTime
  _time = millis();
  uint32_t gap = _time - previousTime;
  if (gap >= FLICKER_RATE) {
    previousTime = _time;
    return true;
  }
  else {
    // this includes when millis() rolls over to 0, because gap will be < 0 
    return false;
  }
}

bool LedString::isEventTime() {
  return isEventTime(_lastEventTime);
}

int numSwitches = 0;
int switchIndex = 0;
int switchIndexToToggle = 0;
uint32_t nextSwitchTime = 0;
uint32_t switchInterval = 0;

void LedString::setupSwitches() {
  for (int i = 0; i < _length; i++) {
    if (pattern.charAt(i) == 'S') {
      numSwitches++;
    }
  }
  if (numSwitches > 1) {
    switchIndexToToggle = random(0, numSwitches);
    switchInterval = (max(LedString::AVERAGE_SWITCH_INTERVAL / numSwitches, LedString::MIN_SWITCH_INTERVAL));
    nextSwitchTime = _time + switchInterval;
  }
}

void checkSwitch(CRGB* leds, int i) {
  // skip if it's not time to switch
  if (_time < nextSwitchTime) return;
  if (switchIndex == switchIndexToToggle) {
    // toggle this switch, pick another to toggle next, and reset the index
    if (leds[i].red > 0) {
      leds[i] = CRGB::Black;
    } else {
      leds[i] = CRGB::White;
    }
    switchIndexToToggle = random(0, numSwitches);
    nextSwitchTime = _time + switchInterval;
    switchIndex = 0;
  } else {
      switchIndex++;
  }
}

void LedString::setupStandardHandlers() {
  setHandler('R', &setRed);
  setHandler('G', &setGreen);
  setHandler('B', &setBlue);
  setHandler('Y', &setYellow);
  setHandler('W', &setWhite);
  setHandler('O', &setBlack);  // O=Off
  setHandler('F', &flicker);
  setHandler('S', &checkSwitch);
}

void LedString::addBehavior(char label) {
  for (int i=0; i<handler_count; i++) {
    if (labels[i] == label) {
      behaviors[behavior_count++] = handlers[i];
      return;
    }
  }
}

void LedString::parsePattern(String pat) {
  behavior_count = 0;
  for (int i = 0; i < _length; i++) {
    char label = pat.charAt(i);
    addBehavior(label);
  }
}

void LedString::setPattern(String newPattern) {
  // if new string is longer than original it is truncated;
  // if shorter it is padded with Os to turn off the unused leds.
  String st = newPattern;
  st.replace(" ", "");
  st.remove(_length);
  st.toUpperCase();
  int shortness = _length - st.length();
  for (int i=0; i<shortness; i++) {
    st += "O";
  }
  pattern = st;
}

void LedString::dummyCycleSetup() {
}

void LedString::doCycle() {
  cycleSetup();
  // perform each led behavior, passing it the led number
  for (int i = 0; i < behavior_count; i++) {
    behaviors[i](leds, i);
  }
  FastLED.show();
}

//void LedString::doSetup(String ledPattern) {
//  leds = (CRGB*)malloc(_length * sizeof(CRGB));
//  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, _length);
//  //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, _length);
//  doSetup(ledPattern, leds);
//  doStart();
//}

void LedString::doSetup(String _pattern, CRGB* ledArray) {
  leds = ledArray;
  _length = FastLED.size();
  setupStandardHandlers();
  setCycleSetup(dummyCycleSetup);
  setPattern(_pattern);
}

void LedString::doStart() {
  parsePattern(pattern);
  setupSwitches();
  turnAllOff();
  doCycle();
}

void LedString::doLoop() {
  if (isEventTime()) {
    doCycle();
  }
}