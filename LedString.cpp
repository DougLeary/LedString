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

void LedHandler::start() {
  // if handler is enabled, do before cycling through leds
}

void LedHandler::loop(CRGB* leds, int i) {
  // do for each led with this label
  Serial.println(" base class handler");
}

/////////////// SimpleHandler

SimpleHandler::SimpleHandler(char label, uint32_t interval, CRGB::HTMLColorCode color)
    : LedHandler(label, interval)
{
  this->color = color;
}

void SimpleHandler::loop(CRGB* leds, int i) {
  Serial.print("Setting led "); 
  Serial.print(i);
  Serial.print(" to color ");
  Serial.println(color);
  leds[i] = color;
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
      Serial.print("Replacing handler "); Serial.println(h->label);
      handlers[i] = h;
      return;
    }
  }
  // not found so append
  Serial.print("Adding handler "); Serial.println(h->label);
  handlers[handler_count] = h;
  handler_count++;
}

void LedString::addSimpleHandler(char label, uint32_t interval, CRGB::HTMLColorCode color)
{
  SimpleHandler* h = new SimpleHandler(label, interval, color);
  addHandler(h);
}

void LedString::setLed(int i, CRGB::HTMLColorCode color) {
  leds[i] = color;
}

////// standard handlers
void setRed(CRGB* leds, int i) {
  Serial.print(" Red");
  leds[i] = CRGB::Red;
}

void setGreen(CRGB* leds, int i) {
  Serial.print(" Green");
  leds[i] = CRGB::Green;
}

void setBlue(CRGB* leds, int i) {
  Serial.print(" Blue");
  leds[i] = CRGB::Blue;
}

void setYellow(CRGB* leds, int i) {
  Serial.print(" Yellow");
  leds[i] = CRGB::Yellow;
}

void setWhite(CRGB* leds, int i) {
  Serial.print(" White");
  leds[i] = CRGB::White;
}

void setBlack(CRGB* leds, int i) {
  Serial.print(" Black");
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

void LedString::countSwitches() {
  Serial.println("Counting switches");
  for (int i = 0; i < _length; i++) {
    if (pattern.charAt(i) == 'S') {
      numSwitches++;
    }
  }
  // Serial.printf("%n switches\n", numSwitches);
  // if (numSwitches > 1) {
  //   switchIndexToToggle = random(0, numSwitches);
  //   switchInterval = (max(LedString::AVERAGE_SWITCH_INTERVAL / numSwitches, LedString::MIN_SWITCH_INTERVAL));
  //   nextSwitchTime = _time + switchInterval;
  // }
  Serial.printf("%d Switches\n", numSwitches);
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

void LedString::addBuiltInHandlers() {
  addSimpleHandler('O', 0, CRGB::Black);    // also used as the dummy handler for unknown pattern chars
  addSimpleHandler('R', 0, CRGB::Red);
  addSimpleHandler('G', 0, CRGB::Green);
  addSimpleHandler('B', 0, CRGB::Blue);
  addSimpleHandler('Y', 0, CRGB::Yellow);
  addSimpleHandler('W', 0, CRGB::White);
//  addHandler('F', 0, &flicker, FLICKER_RATE);
//  addHandler('S', 0, &checkSwitch, 0);
  Serial.print("Standard Handlers Added: ");
  Serial.println(handler_count);
  char ch = handlers[handler_count - 1]->label;
  Serial.printf("Last one is %c \n",ch);

}

void LedString::addBehavior(char label) {
  // find the handler for the label and append it to behaviors
  for (int i=0; i<handler_count; i++) {
    if (handlers[i]->label == label) {
      Serial.print("Using behavior ");
      Serial.println(label);
      behaviors[behavior_count++] = handlers[i];
      return;
    }
  }
  Serial.print("Behavior "); Serial.print(label); Serial.println(" not found; using default");
  behaviors[behavior_count++] = handlers[0];  // use dummy handler
}

void LedString::populateBehaviors() {
  // for each character in the pattern add the corresponding handler to the behaviors list
  behavior_count = 0;
  Serial.printf("Populating %d Behaviors for pattern ", _length);
  Serial.println(pattern);
  for (int i = 0; i < _length; i++) {
    char label = pattern.charAt(i);
    addBehavior(label);
  }
  Serial.print(behavior_count); Serial.println(" behaviors added");
}

void LedString::setPattern(String newPattern) {
  // if new string is longer than original it is truncated;
  // if shorter it is padded with Os to turn off the unused leds.
  Serial.print("newPattern: "); Serial.println(newPattern);
  String st = newPattern;
  Serial.print("st: "); Serial.println(st);
  // st.replace(" ", "");
  // st.remove(_length);
  // st.toUpperCase();
  //int shortness = _length - st.length();
  // for (int i=0; i<shortness; i++) {
  //   st += "O";
  // }
  pattern = st;
  _length = pattern.length();
  Serial.print("pattern: "); Serial.println(pattern);
  populateBehaviors();
}

void LedString::enableHandlers() {
  // based on current time and interval, enable handlers that should execute and run their cycle start functions
  Serial.println("===== enableHandlers");
  for (int i=0; i < handler_count; i++) {
    LedHandler* h = handlers[i];
    h->enabled = (isEventTime(h->interval, h->whenLast));
    Serial.print(i);
    if (h->enabled) {
      Serial.print(" "); Serial.print(h->label); Serial.println(" enabled");
      h->whenLast = _time;
      h->start();
    } else { Serial.println(" disabled"); }
  }
}

void LedString::doBehaviors() {
  Serial.println("===== doBehaviors");
  for (int i = 0; i < behavior_count; i++) {
    LedHandler* h = behaviors[i];
    if (h->enabled) {
      Serial.print("Doing ");
      Serial.print(h->label);
      h->loop(leds, i);
      Serial.println();
    } else { 
      Serial.print(h->label); Serial.println(" not enabled");
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
  Serial.print("Calling setPattern for ");
  Serial.println(_pattern);
  setPattern(_pattern);
  Serial.println("pattern set");
  countSwitches();
  Serial.println("setup Done");
}

void LedString::loop() {
  _time = millis();
  enableHandlers();
  doBehaviors();
  FastLED.show();
}