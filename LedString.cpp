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

void LedString::resetAll() {
  FastLED.clear();
}

void LedString::flicker(int led) {
  int value = random(FIRE_MIN, FIRE_MAX);
  // occasional intense flicker
  if (value <= FIRE_MIN + FLICKER_EXTRA) {
    value = FIRE_MIN - ((FIRE_MIN - FLICKER_MIN) / (value - FIRE_MIN + 1));
  }
  else if (value >= FIRE_MAX - FLICKER_EXTRA) {
    value = FIRE_MAX + ((FLICKER_MAX - FIRE_MAX) / (FIRE_MAX - value));
  }
  leds[led] = CHSV(25, 187, value);
  FastLED.show();
}

void LedString::setCustom(LedStringCustomFunction f) {
  customHandler = f;
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
  leds[led] = CRGB::White;
  FastLED.show();
}

void LedString::turnOff(int led) {
  leds[led] = CRGB::Black;
  FastLED.show();
}

void LedString::turnAllOn() {
  for (int i = 0; i < _length; i++) {
    leds[i] = CRGB::White;
  }
  FastLED.show();
}

void LedString::turnAllOff() {
  for (int i = 0; i < _length; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void LedString::turnOnSteadies() {
  // turn on all switched or constantly lit leds
  for (int i = 0; i < _length; i++) {
    char ch = pattern.charAt(i);
    switch (ch) {
    case 'W':
      leds[i] = CRGB::White;
      break;
    case 'S':
      leds[i] = CRGB::White;
      break;
    case 'R':
      leds[i] = CRGB::Red;
      break;
    case 'G':
      leds[i] = CRGB::Green;
      break;
    case 'B':
      leds[i] = CRGB::Blue;
      break;
    case 'Y':
      leds[i] = CRGB::Yellow;
      break;
    }
  }
  FastLED.show();
}

bool LedString::isEventTime() {
  long msNow = millis();
  long gap = msNow - lastEventTime;
  if (gap >= FLICKER_RATE) {
    lastEventTime = msNow;
    return true;
  }
  else {
    // note: this includes when millis() rolls over to 0, because gap will be < 0 
    return false;
  }
}

void LedString::setupSwitches() {
  for (int i = 0; i < _length; i++) {
    if (pattern.charAt(i) == 'S') {
      numSwitches++;
    }
  }
  if (numSwitches > 1) {
    switchIndexToToggle = random(0, numSwitches);
    nextSwitchTime = millis() + (max(AVERAGE_SWITCH_INTERVAL / numSwitches, MIN_SWITCH_INTERVAL));
  }
}

void LedString::checkSwitch(int led) {
  // skip if it's not time to switch
  long msNow = millis();
  if (msNow < nextSwitchTime) return;
  if (switchIndex == switchIndexToToggle) {
    // toggle this switch, pick another to toggle next, and reset the index
    if (isOn(led)) {
      turnOff(led);
    }
    else {
      turnOn(led);
    }
    switchIndexToToggle = random(0, numSwitches);
    nextSwitchTime = msNow + nextSwitchTime;
    switchIndex = 0;
  } else {
      switchIndex++;
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
  setupSwitches();
  doStart();
}

void LedString::dummyCustom(CRGB led) {
}

void LedString::dummyCycleSetup() {
}

void LedString::doCycle() {
  cycleSetup();
  long msNow = millis();
  for (int i = 0; i < _length; i++) {
    char ch = pattern.charAt(i);
    switch (ch) {
    case 'S':
      checkSwitch(i);
      break;
    case 'F':
      flicker(i);
      break;
    case 'C':
      customHandler(i);
      break;
    }
  }
}

void LedString::doStart() {
  turnAllOff();
  turnOnSteadies();
  doCycle();
}

void LedString::doSetup(String _pattern, CRGB* ledArray) {
  leds = ledArray;
  _length = FastLED.size();
  setCustom(dummyCustom);
  setCycleSetup(dummyCycleSetup);
  setPattern(_pattern);
}

//void LedString::doSetup(String ledPattern) {
//  leds = (CRGB*)malloc(_length * sizeof(CRGB));
//  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, _length);
//  //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, _length);
//  doSetup(ledPattern, leds);
//  doStart();
//}

void LedString::doLoop() {
  if (isEventTime()) {
    doCycle();
  }
}
