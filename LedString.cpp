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

/////////////// ActiveGroup

ActiveGroup::ActiveGroup(char label, uint32_t minInterval, uint32_t maxInterval, CRGB color, int percentOn)
    : LedHandler(label, 0)
{
  this->minInterval = minInterval;
  this->maxInterval = max(maxInterval, minInterval+1);  // in case minInterval = maxInterval
  this->color = color;
  this->percentOn = percentOn;
}

void ActiveGroup::setup(LedString ls) {
  // count the leds in this group and turn each one on/off randomly according to percentOn
  groupCount = 0;
  groupCountOn = 0;
  for (int i = 0; i < ls.ledCount; i++)
  {
    if (ls.pattern.charAt(i) == this->label) {
      groupCount++;
      if (random(0, 100) < percentOn) {
        ls.leds[i] = color;
        groupCountOn++;
      } else {
        ls.leds[i] = CRGB::Black;
      }
    }
  }
  isTurningOn = (percentOn >= 50);  // will be reversed when start() first executes
}

void ActiveGroup::start(LedString ls) {
  isTurningOn = (groupCountOn < (ls.ledCount * percentOn / 100));

  if (isTurningOn) {
    countdown = random(0, groupCount - groupCountOn);
  } else {
    countdown = random(0, groupCountOn);
  }
}

void ActiveGroup::loop(LedString ls, int ledNumber) {
  bool isOn = ls.isOn(ledNumber);
  if (isTurningOn && !isOn) {
    if (countdown > 0) {
      countdown--;
    } else {
      ls.leds[ledNumber] = color;
      groupCountOn++;
      enabled = false;
      interval = random(minInterval, maxInterval);
    }
  } else if (!isTurningOn && isOn) {
    if (countdown > 0)
    {
      countdown--;
    }
    else
    {
      ls.leds[ledNumber] = CRGB::Black;
      groupCountOn--;
      enabled = false;
      interval = random(minInterval, maxInterval);
    }
  } else {
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

bool LedString::isOn(int led) {
  return (
    (leds[led].red > 0) ||
    (leds[led].green > 0) ||
    (leds[led].blue > 0));
}

void LedString::turnOn(int i) {
  leds[i] = CRGB::White;
}

void LedString::turnOff(int i) {
  leds[i] = CRGB::Black;
}

void LedString::turnAllOn() {
  for (int i = 0; i < ledCount; i++) {
    leds[i] = CRGB::White;
  }
}

void LedString::turnAllOff() {
  for (int i = 0; i < ledCount; i++) {
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

void LedString::addBuiltInHandlers() {
  addSimpleHandler('O', 0, CRGB::Black);    // also used as the dummy handler for unknown pattern chars
  addSimpleHandler('R', 0, CRGB::Red);
  addSimpleHandler('G', 0, CRGB::Green);
  addSimpleHandler('B', 0, CRGB::Blue);
  addSimpleHandler('Y', 0, CRGB::Yellow);
  addSimpleHandler('W', 0, CRGB::White);
  FlameHandler *fh = new FlameHandler('F', FLICKER_RATE);
  addHandler(fh);
  ActiveGroup *ag = new ActiveGroup('A', HABITATION_MIN_INTERVAL, HABITATION_MAX_INTERVAL, CRGB::White, HABITATION_DEFAULT_PERCENT);
  addHandler(ag);
}

void LedString::addBehavior(char label, int ledNumber) {
  // find the handler for the label and append it to behaviors
  for (int i=0; i<handler_count; i++) {
    if (handlers[i]->label == label) {
      behaviors[ledNumber] = handlers[i];
      return;   
    }
  }
  behaviors[ledNumber] = handlers[0];  // use dummy handler
}

void LedString::populateBehaviors() {
  // for each character in the pattern add the corresponding handler to the behaviors list
  for (int i = 0; i < ledCount; i++) {
    char label = pattern.charAt(i);
    addBehavior(label, i);
  }
}

void LedString::setupHandlers() {
  for (int i = 0; i < handler_count; i++)
  {
    handlers[i]->setup(*this);
  }
}

void LedString::setPattern(String newPattern) {
  // if new string is longer than original it is truncated;
  // if shorter it is padded with Os to turn off the unused leds.
  pattern = newPattern;
  pattern.replace(" ", "");
  pattern.remove(ledCount);
  int shortness = ledCount - pattern.length();
  for (int i=0; i<shortness; i++) {
    pattern += "O";
  }
  pattern.toUpperCase();
  setupHandlers();
  populateBehaviors();
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

  for (int i = 0; i < ledCount; i++) {
    LedHandler* h = behaviors[i];
    if (h->enabled) {
      h->loop(*this, i);
    }
  }
}

//void LedString::setup(String ledPattern, int ledCount) {
//  leds = (CRGB*)malloc(ledCount * sizeof(CRGB));
//  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, ledCount);
//  //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, ledCount);
//  setup(ledPattern, leds);
//}

void LedString::setup(CRGB *ledArray, int numLeds) {
  leds = ledArray;
  ledCount = numLeds;
  turnAllOff();
  addBuiltInHandlers();
}

void LedString::begin(String newPattern) {
  setPattern(newPattern);
}

void LedString::loop() {
  _time = millis();
  enableHandlers();
  doBehaviors();
  FastLED.show();
}