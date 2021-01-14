/*
LedString class by Doug Leary 2020
Simple FastLED consumer to control lighting in model buildings for a Christmas village, railroad layout or the like.

Tested Controllers:
  Arduino Uno 
  NodeMCU 1.0 (ESP-12E)
  ESP-01s
Tested LED devices:
  WS2811
  NEOPIXEL
*/

#include "FastLED.h"
#include "LedString.h"

//////////// LedHandler

LedHandler::LedHandler(char label, uint32_t interval)
{
  this->label = label;
  this->interval = interval;
}

void LedHandler::setup(LedString ls)
{
  // do when a pattern is created or changed
}

void LedHandler::start(LedString ls)
{
  // if handler is enabled, do before cycling through leds
}

void LedHandler::loop(LedString ls, int i)
{
  // do for each led with this label
}

/////////////// SimpleHandler

SimpleHandler::SimpleHandler(char label, uint32_t interval, CRGB color)
  : LedHandler(label, interval)
{
  this->color = color;
}

void SimpleHandler::loop(LedString ls, int i)
{
  ls.leds[i] = color;
}

/////////////// Flame

FlameHandler::FlameHandler(char label, uint32_t interval) 
  : LedHandler(label, interval)
{ }

void FlameHandler::loop(LedString ls, int i)
{
  int value = random(LedString::FIRE_MIN, LedString::FIRE_MAX);
  // occasional intense flicker
  if (value <= LedString::FIRE_MIN + LedString::FLICKER_EXTRA)
  {
    value = LedString::FIRE_MIN - ((LedString::FIRE_MIN - LedString::FLICKER_MIN) / (value - LedString::FIRE_MIN + 1));
  }
  else if (value >= LedString::FIRE_MAX - LedString::FLICKER_EXTRA)
  {
    value = LedString::FIRE_MAX + ((LedString::FLICKER_MAX - LedString::FIRE_MAX) / (LedString::FIRE_MAX - value));
  }
  ls.leds[i] = CHSV(25, 187, value);
};

/////////////// SwitchGroup

SwitchGroup::SwitchGroup(char label, uint32_t minInterval, uint32_t maxInterval, CRGB color)
  : LedHandler(label, 0)
{
  this->minInterval = minInterval;
  this->maxInterval = maxInterval;
  this->color = color;
}

void SwitchGroup::setup(LedString ls)
{
  // count the switched leds in this group
  switchCount = 0;
  countdown = random(switchCount);
  int len = ls.pattern.length();
  for (int i = 0; i < len; i++)
  {
    if (ls.pattern.charAt(i) == this->label)
    {
      switchCount++;
      ls.leds[i] = color;
    }
  }
}

void SwitchGroup::loop(LedString ls, int ledNumber)
{
  // toggle a led when the countdown reaches 0
  if (countdown > 0)
  {
    countdown--;
  }
  else
  {
    if (ls.leds[ledNumber] == color)
    {
      ls.leds[ledNumber] = CRGB::Black;
    }
    else
    {
      ls.leds[ledNumber] = color;
    }
    // set random time and led for next toggle
    enabled = false; // once we've toggled, disable until the next cycle
    countdown = random(switchCount);
    interval = random(minInterval, maxInterval);
  }
}

/////////////// LedString

uint32_t _time = 0;
uint32_t _previousTime = 0;

uint32_t LedString::currentTime() {
  return _time;
}

uint32_t LedString::previousTime() {
  return _previousTime;
}

// array of defined handlers the system knows about
LedHandler* handlers[MAX_LED_STRING_HANDLERS];
int handler_count = 0;

// behaviors is an array of handlers, one for each led;
LedHandler* behaviors[MAX_LEDS];
int behavior_count = 0;

void LedString::addHandler(LedHandler* h) {
  // if exists replace
  for (int i=0; i<handler_count; i++) {
    if (handlers[i]->label == h->label) {
      handlers[i] = h;
      return;
    }
  }
  // not found so append
  handlers[handler_count] = h;
  handler_count++;
}

void LedString::addSimpleHandler(char label, uint32_t interval, CRGB color)
{
  SimpleHandler* h = new SimpleHandler(label, interval, color);
  addHandler(h);
}

void LedString::setLed(int i, CRGB color) {
  leds[i] = color;
}

void LedString::resetAll() {
  FastLED.clear();
}

bool LedString::isOn(int led) {
  return (
    (leds[led].red == 255) &&
    (leds[led].green == 255) &&
    (leds[led].blue == 255));
}

void LedString::turnOn(int i) {
  leds[i] = CRGB::White;
}

void LedString::turnOff(int i) {
  leds[i] = CRGB::Black;
}

void LedString::turnAllOn() {
  for (int i = 0; i < _length; i++) {
    leds[i] = CRGB::White;
  }
}

void LedString::turnAllOff() {
  for (int i = 0; i < _length; i++) {
    leds[i] = CRGB::Black;
  }
}

bool LedString::isEventTime(uint32_t interval, uint32_t &previousTime) {
  if (_time - previousTime >= interval) {
    previousTime = _time;
    return true;
  }
  else {
    // this includes when millis() rolls over to 0, because gap will be < 0 
    return false;
  }
}

int numSwitches = 0;
int switchIndex = 0; 
int switchIndexToToggle = 0;
uint32_t nextSwitchTime = 0;
uint32_t switchInterval = 0;

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

void LedString::addBuiltInHandlers() {
  addSimpleHandler('O', 0, CRGB::Black);    // also used as the dummy handler for unknown pattern chars
  addSimpleHandler('R', 0, CRGB::Red);
  addSimpleHandler('G', 0, CRGB::Green);
  addSimpleHandler('B', 0, CRGB::Blue);
  addSimpleHandler('Y', 0, CRGB::Yellow);
  addSimpleHandler('W', 0, CRGB::White);
  FlameHandler *fire = new FlameHandler('F', FLICKER_RATE);
  addHandler(fire);
}

void LedString::addBehavior(char label) {
  // find the handler for the label and append it to behaviors
  for (int i=0; i<handler_count; i++) {
    if (handlers[i]->label == label) {
      behaviors[behavior_count++] = handlers[i];
      return;
    }
  }
  behaviors[behavior_count++] = handlers[0];  // use dummy handler
}

void LedString::populateBehaviors() {
  // for each character in the pattern add the corresponding handler to the behaviors list
  behavior_count = 0;
  for (int i = 0; i < _length; i++) {
    char label = pattern.charAt(i);
    addBehavior(label);
  }
}

void LedString::setPattern(String newPattern) {
  // if new string is longer than original it is truncated;
  // if shorter it is padded with Os to turn off the unused leds.
  String st = newPattern;
  // st.replace(" ", "");
  // st.remove(_length);
  // st.toUpperCase();
  //int shortness = _length - st.length();
  // for (int i=0; i<shortness; i++) {
  //   st += "O";
  // }
  pattern = st;
  _length = pattern.length();
  populateBehaviors();
  for (int i = 0; i < handler_count; i++)
  {
    handlers[i]->setup(*this);
  }
}

void LedString::enableHandlers() {
  // based on current time and interval, enable handlers that should execute and run their cycle start functions
  for (int i=0; i < handler_count; i++) {
    LedHandler* h = handlers[i];
    h->enabled = (isEventTime(h->interval, h->whenLast));
    if (h->enabled) {
      h->whenLast = _time;
      h->start(*this);
    }
  }
}

void LedString::doBehaviors() {
  for (int i = 0; i < behavior_count; i++) {
    LedHandler* h = behaviors[i];
    if (h->enabled) {
      h->loop(*this, i);
    }
  }
}

//void LedString::setup(String ledPattern) {
//  leds = (CRGB*)malloc(_length * sizeof(CRGB));
//  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, _length);
//  //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, _length);
//  setup(ledPattern, leds);
//}

void LedString::setup(CRGB* ledArray) {
  leds = ledArray;
  turnAllOff();
  addBuiltInHandlers();
}

void LedString::begin(String _pattern) {
  setPattern(_pattern);
}

void LedString::loop() {
  _time = millis();
  enableHandlers();
  doBehaviors();
  FastLED.show();
}