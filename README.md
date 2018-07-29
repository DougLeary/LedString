# LedString
FastLED wrapper to simplify lighting for model towns, castles, villages.

This wrapper class uses FastLED to set up WS2811 lighting for buildings in a model town, medieval village, etc. 
This class does not do animations as such; it assigns a predefined behavior to each led, and that led is lit accordingly.

#### Behaviors
L = always Lit (white)  
O = always Off (black) - in other words, not used (uppercase "O" not zero)  
S = Switched on and off at random to make the place look inhabited  
F = Flickering yellow light to simulate fireplaces, campfires, etc. Fire LEDs flicker independently.   
C = Custom behavior

Behavior for individual LEDs is defined using a character string, one character per LED. Blanks can be inserted anywhere for readability. Example:
```
  lights.doSetup("LLOFL SSFLO OOLSS FSSSL");"LLOFL SSFLO OOLSS FSSSL"  
```
    
This defines a behavior pattern for 20 LEDs.  
  First two (LL): constantly lit
  Third (O): always off
  Fourth (F): flickering fire
  Fifth (L): constantly lit
  ... and so on
  
The length of the text string is used to allocate the CRGB array dynamically. Dynamic allocation is generally frowned upon 
in Arduino code because of the potential for memory fragmentation, but this step is only executed once in the sketch setup.

The original goal was to control multiple strings of leds with a single Arduino or ESP8266, using a different DATA_PIN for each string. However, FastLED requires specifying DATA_PIN as a constant for purposes of compile-time optimization. 
As of this writing there is no way for DATA_PIN to be a property that would vary from one object instance to another. 
So for now each string of leds needs its own controller, and an app can only use one instance of LedString. 

Running multiple strings on one controller should still be possible if an external multiplexing circuit is used to connect selected LED strings to DATA_PIN, using a couple other pins for addressing, but I have not tried to do this yet. 

### Usage Example

```
#include "FastLED.h"

LedString lights;

void setup() {
  lights.doSetup("LLOFL SSFLO OOLSS FSSSL");    // define LED behaviors
  lights.doStart();                             // set Lit and Off leds, which won't change;
                                                // turn Switched leds on
} 

void loop() {
  lights.doLoop();      // change the values of Fires, Swiched leds and Customs if it's time to do so
}
```

## Custom Behavior
Custom behavior is a limited way to add more lighting options. If you write a void function that takes one int argument (the led number), and call **lights.setCustom**(yourFunction), your function will be called for every led whose behavior is specified with a "C". There are a few utility functions in LedString you can use within your function, such as turnOn(led) and turnOff(led), and you can reference lights.leds[n] for calling CRGB and CHSV functions (see examples/CustomBehavior).  

## Notes
1. To control a 50-led WS2811 string, the wiring is straightforward: connect the LED string's control line to pin 3, connect an adequate 5V power supply to the LEDs, and tie the power supply's DC ground to the Arduino ground.  

2. Small sections of LED strings can be run off Arduino power alone. I don't know the limit, but I have run a string of 8 leds cut from a string of 50. 

3. All fire LEDs are updated at the same regular interval, but this pattern is not apparent because each fire's brightness changes by a random amount. 

4. The fire flicker interval acts as the master timing interval for the LED refresh cycle, even if no LEDs are designated as fires. This is simply because firelight was the first thing I implemented. The Arduino doLoop() checks whether it's time to flicker. If so, it makes a pass through the behavior string and updates fire leds and any other leds that need updating at that time. Therefore no other effects can change more frequently than the flickering rate of fires, and all effects occur at some multiple of the flicker rate. This limitation should not be a problem, given that the purpose of this package is to handle slow-paced lighting for towns, villages and such. 
