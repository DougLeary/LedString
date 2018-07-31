# LedString
FastLED wrapper to simplify lighting for model towns, castles, villages.

This wrapper class uses FastLED to set up WS2811 lighting for buildings in a model town, medieval village, etc. 
This class does not do animations as such; it assigns a predefined behavior to each led, and that led is lit accordingly.

#### Behaviors
L = always Lit (white)  
O = always Off (black) - in other words, not used (uppercase "O" not zero)  
S = Switched on and off semi-randomly to give an appearance of habitation.  
F = Flickering yellow light to simulate fireplaces, campfires, etc. Fire LEDs flicker independently.   
C = Custom behavior

Behavior for individual LEDs is defined using a character string, one character per LED. Blanks can be inserted anywhere for readability. Example: ```  lights.doSetup("LLOFL SSFLO OOLSS FSSSL");```
    
This example defines a behavior pattern for 20 LEDs.  
  First two (LL): constantly lit  
  Third (O): always off  
  Fourth (F): flickering fire  
  Fifth (L): constantly lit  
  ... and so on
  
The length of the text string is used to allocate the CRGB array dynamically. Dynamic allocation is generally frowned upon 
in Arduino code because of the potential for memory fragmentation, but this step is only executed once in the sketch setup.

This code works on Arduino and ESP9266. Connect your LED string's data line to pin 3 on an Arduino, or pin D3 on an ESP8266. 

The original goal was to control multiple strings of leds with a single controller, using a different DATA_PIN for each string. However, FastLED requires specifying DATA_PIN as a constant for purposes of compile-time optimization. 
As of this writing there is no way for DATA_PIN to be a property that would vary from one object instance to another. 
So for now each string of leds needs its own controller, and an app can only use one instance of LedString. 

Running multiple strings on one controller should still be possible by using an external multiplexing circuit to connect them to DATA_PIN, using a couple other pins for addressing, but I have not tried to do this yet. 

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
Custom behavior is a limited way to add more lighting options (see examples/CustomBehavior). Basically you designate custom LEDs with the letter "C" in doSetup(behaviorString), and write a function like ```void customBehavior(int ledNumber)``` that implements the behaviors. Then in setup() call **lights.setCustom**(customBehavior). Your function will be called for every led whose behavior is specified with a "C". The LedString package has utility functions you can use such as turnOn(led) and turnOff(led), and you can reference LEDs as lights.leds[n] to call CRGB and CHSV functions.  

## Notes
1. To control a 50-led WS2811 string, the wiring is straightforward: connect the LED string's control line to pin 3, connect an adequate 5V power supply to the LEDs, and tie the power supply's DC ground to the Arduino ground.  

2. Small sections of LED strings can be run off Arduino power alone. I don't know the limit, but I have run a string of 8 leds cut from a string of 50. 

3. All fire LEDs are updated at the same regular interval, but this pattern is not apparent because each fire's brightness changes by a random amount. 

4. The fire flicker interval acts as the master timing interval for the LED refresh cycle, even if no LEDs are designated as fires. This is simply because firelight was the first thing I implemented. The Arduino doLoop() checks whether it's time to flicker. If so, it makes a pass through the behavior string and updates fire leds and any other leds that need updating at that time. This means all effects occur at some multiple of the flicker rate. I figured this limitation was acceptable in a package designed to handle slow-paced lighting for towns, villages and such. 
