## READ THIS FIRST  
This project has been neglected for several years. The README doesn't include major improvements in adding custom behaviors, and I see a few other omissions, errors and awkward wording. Same with some of the code examples, which I think are cryptic due to lack of comments. I'm in the process of making code improvements, better examples, and bringing the README completely up to date - ETA early Feb 2026. This library isn't designed for spectacular animations or dazzling effects, it's goal is simplicity - to make it dead easy to use FastLED for simple tasks, while retaining some versatility, to control programmable RGB lighting in model buildings, railroad layouts and the like. If this interests you, please check back in February. Thanks! - DL

-------

# LedString
A wrapper class for [FastLED](https://fastled.io/) to simplify proammable LED lighting. I use it for a model Christmas town display, but it would work for anything similar - castles, villages, model train layouts, etc. It wasn't designed to generally animate Christmas lights, but who knows, give it a try. LEDString Has been tested with WS2811, WS2812, WS2812B, WS2813, and NEOPIXEL rings and matrices.

Behaviors are assigned to a string of programmable LEDs using a simple text string, one character per LED. Blanks can be included anywhere for readability, and are ignored. For example, the behavior string for two small cottages, each lit by a flickering fire, next to a brightly lit candy store with 3 white leds, could be "F F WWW". This is the same as "FFWWW" but a little more readable. When you edit a long string that defines dozens of leds, blanks make it easier to find the right section.

#### Built-in Behaviors  
**Static Colors**  
* W: White
* R: Red
* G: Green
* B: Blue
* Y: Yellow
* O: Off

**Animated**  
* F: Fire (a flickering simulation of a fireplace, torch, etc). Fire LEDs flicker independently of each other.
* A: Active group; leds marked "A" are switched on and off at semi-random times, to create the feel of places being inhabited.
At a random interval between SWITCH_MIN and SWITCH_MAX milliseconds, one "A" led is randomly chosen and toggled on or off.

**Custom Behaviors**  
Using the Custom class you can define a custom behavior, assign it a letter, and use that letter to assign the behavior to leds. 
See [examples/Custom/Custom.ino](https://github.com/DougLeary/LedString/blob/master/examples/Custom/Custom.ino).
You can even override a built-in behavior by reassigning its letter to a new behavior. 

The constant NUM_LEDS must be defined as per the FastLED docs. If the pattern string (excluding blanks) is longer or shorter than NUM_LEDS, it is truncated or padded with the letter "O" respectively. 

Note: As per FastLED docs, the value for DATA_PIN used to call addLeds must be a _constant_ (or multiple constants for multiple led strings). 
This code works on Arduino and ESP8266.

Since only one "S" node is toggled on or off per interval, the more "S" leds you use, the less often an individual one will be switched. So if you want the appearance of more activity, edit LedString.h and try assigning a lower value for SWITCH_MAX. 

The led type is assumed to be WS2811 or WS2812. To select a different type you must make two edits: 
- in LedString.h: uncomment the appropriate line to set FIRE_MIN, etc for the type of leds you are using.
- in LedString.cpp: uncomment the appropriate call to FastLED.addLeds for your type of leds. 

These edits are necessary because of FastLED reqirements, which LedString is based on. Due to compile-time optimizations in FastLED, certain values cannot be passed in as parameters. 

### Usage Example

```
#include "LedString.h"

LedString lights;

void setup() {
  lights.doSetup("LLOFL SSFLO OOLSS FSSSL");    // define LED behaviors
  lights.doStart();                             // set Lit and Off leds that don't change;
                                                // turn Switched leds on
} 

void loop() {
  lights.doLoop();      // change the values of Fires, Swiched leds and Customs whenever it is time to do so
}
```

## Custom Behavior
Custom behavior is a limited way to add more lighting options (see examples/CustomBehavior). Basically you designate custom LEDs with the letter "C" in doSetup(behaviorString), and write a function such as ```void myCustom(int ledNumber)``` that implements the behaviors. Then in setup() call **lights.setCustom**(myCustom). Your function will be called for every led whose behavior is specified with a "C". The LedString package has utility functions you can use such as turnOn(led) and turnOff(led), and you can reference LEDs as lights.leds[n] to call CRGB and CHSV functions. If you need to do something at the start of cycling through the leds, such as resetting variables used in custom behaviors, put that code in a function such as ```void myCycleSetup()``` and in setup() call **lights.setCycleSetup**(myCycleSetup). Your function will be called every time the main loop starts cycling through the leds.

## Notes
1. To control a 50-led string, the wiring is straightforward: connect the LED string's control line to pin 3 (or whatever you you designate as DATA_PIN, connect an adequate 5V power supply to the LEDs, and tie the power supply's DC ground to the Arduino ground.     

2. Small sections of LED strings can be run off Arduino power alone. I don't know the limit, but I have run a string of 8 leds cut from a string of 50. 

3. All fire LEDs are updated at the same regular interval, but this pattern is not apparent because each fire's brightness changes by a random amount. 

4. The fire flicker interval acts as the master timing interval for the LED refresh cycle, even if no LEDs are designated as fires. This is simply because firelight was the first thing I implemented. The Arduino doLoop() checks whether it's time to flicker. If so, it makes a pass through the behavior string and updates fire leds and any other leds that need updating at that time. This means all effects occur at some multiple of the flicker rate. I figured this limitation was acceptable in a package designed to handle slow-paced lighting for towns, villages, Christmas trees and such. A tree full of flickering fire leds would be interesting - I have not tried that, but in my tests 50 fires look fine with an 80ms flicker rate.  

