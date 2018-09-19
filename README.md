# LedString
FastLED wrapper to simplify lighting for model towns, castles, villages.

This wrapper class uses FastLED to set up lighting for buildings in a model town, medieval village, etc. 
LEDs are animated individually by assigning a predefined behavior to each led. The code can be used with WS2811, WS2812, WS2812B, WS2813, or NEOPIXEL rings and matrices. 

#### Behaviors
L = always Lit (white)  
O = always Off (black) - in other words, not used (uppercase "O" not zero)  
S = Switched on and off semi-randomly to give an appearance of habitation. At a random interval of SWITCH_MIN..SWITCH_MAX milliseconds, one randomly chosen led marked "S" is toggled on or off.   
F = Flickering yellow light to simulate fireplaces, campfires, etc. Fire LEDs flicker independently of each other.   
C = Custom behavior

Behavior for individual LEDs is defined using a character string, one character per LED. Blanks can be inserted anywhere for readability.

Example: ```  lights.doSetup("LLOFL SSFLO OOLSS FSSSL");```
    
This example defines a behavior pattern for 20 LEDs.  
  1,2 (LL): constantly lit  
  3 (O): always off  
  4 (F): flickering fire  
  5 (L): constantly lit  
  6,7 (SS): turned on and off semi-randomly  
  ... and so on
  
The length of the text string determines the number of LEDs animated. If there are more LEDs on the string, they are turned off. [The length of the text string is used to allocate FastLED's internal CRGB array dynamically. Dynamic allocation is generally frowned upon 
in Arduino code because of the potential for memory fragmentation, but since this step is executed only once in the sketch it shouldn't be a problem.]

This code works on Arduino and ESP8266. Connect your LED string's data line to pin 3 on an Arduino, or whichever pin you designate as DATA_PIN. To change this pin you must edit the value of DATA_PIN in LedString.h. 

The default hardware is WS2811. To select different hardware you must make two edits:  
- in LedString.h uncomment the appropriate line to set FIRE_MIN, etc. 
- in LedString.cpp uncomment the appropriate call to FastLED.addLeds for your hardware. 

These edits are necessary because of requirements of the FastLED library, which LedString is based on. Due to compile-time optimizations in FastLED, certain values cannot be passed in as parameters. 

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

4. The fire flicker interval acts as the master timing interval for the LED refresh cycle, even if no LEDs are designated as fires. This is simply because firelight was the first thing I implemented. The Arduino doLoop() checks whether it's time to flicker. If so, it makes a pass through the behavior string and updates fire leds and any other leds that need updating at that time. This means all effects occur at some multiple of the flicker rate. I figured this limitation was acceptable in a package designed to handle slow-paced lighting for towns, villages and such. 
