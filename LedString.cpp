/*
LedString class by Doug Leary 2018
Requires FastLED library
Tested on Arduino Uno and NodeMCU 1.0 (ESP-12E)
*/

#include "FastLED.h"
#include "LedString.h"

CRGB* leds = 0;

void LedString::doSetup(String pattern) {
  _pattern = pattern;
  _pattern.replace(" ", "");
  _pattern.toUpperCase();
  _length = _pattern.length();
  leds = (CRGB*)malloc(_length * sizeof(CRGB));
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, _length);
  setupSwitches();
}

void LedString::flicker(int led) {
  int value = random(FIRE_MIN, FIRE_MAX);
  // occasional intense flicker
  if (value <= FIRE_MIN + FLICKER_EXTRA) {
    value = FIRE_MIN - ((FIRE_MIN - FLICKER_MIN) / (value - FIRE_MIN));
  }
  else if (value >= FIRE_MAX - FLICKER_EXTRA) {
    value = FIRE_MAX + ((FLICKER_MAX - FIRE_MAX) / (FIRE_MAX - value));
  }
  leds[led] = CHSV(25, 187, value);
  FastLED.show();
}

void LedString::doCustom() {
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

void LedString::turnLitOn() {
  // turn on all "Lit" leds
  for (int i = 0; i < _length; i++) {
    if (_pattern.charAt(i) == 'L') {
      leds[i] = CRGB::White;
    }
  }
  FastLED.show();
}

void LedString::turnLitOrSwitchedOn() {
  // turn on all "Lit" or "Switched" leds
  for (int i = 0; i < _length; i++) {
    char type = _pattern[i];
    if ((type == 'L') || (type == 'S')) {
      leds[i] = CRGB::White;
    }
  }
  FastLED.show();
}

bool LedString::isEventTime() {
  long msNow = millis();
  long gap = msNow - lastEventTime;
  if (gap < 0) {
    gap = FLICKER_RATE;
  }
  if (gap >= FLICKER_RATE) {
    lastEventTime = msNow;
    return true;
  }
  else {
    return false;
  }
}

void LedString::checkSwitch(int led) {
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

void LedString::setupSwitches() {
  for (int i = 0; i < _length; i++) {
    if (_pattern.charAt(i) == 'S') {
      numSwitches++;
    }
  }
  switchIndexToToggle = random(0, numSwitches);
  nextSwitchTime = millis();
}

void LedString::doStart() {
  turnAllOff();
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

void LedString::doLoop() {
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
