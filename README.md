# LedString
FastLED wrapper to simplify lighting for model towns, castles, villages.

This wrapper class uses FastLED to set up WS2811 lighting for buildings in a model town, medieval village, etc. 
This class does not do animations as such; it assigns a predefined behavior to each led, and that led is lit accordingly.

#### Behaviors
L = always Lit (white)  
O = always Off (black) - in other words, not used (uppercase "O" not zero)  
S = Switched on and off at random to simulate human activity  
F = Flickering yellow light to simulate fireplaces, campfires, etc. Fire LEDs flicker independently.   
C = Custom behavior

Behavior for the set of LEDs is defined using a character string, one char per LED.  
Example string: "LLOFL SSFLO OOLSS FSSSL"  
This example defines a behavior pattern for 20 LEDs.  
First five: two constantly lit (LL), one turned off (O), flickering fire (F), another constantly lit (L).  
Blanks can inserted anywhere for clarity; they are stripped out during operation.
  
The length of the text string is used to allocate the CRGB array dynamically. Dynamic allocation is generally frowned upon 
in Arduino code because of the potential for memory fragmentation, but this step is only executed once in the sketch setup.

The original goal was to control multiple strings of leds with a single Arduino or ESP8266, using a different DATA_PIN for each string. However, FastLED requires specifying DATA_PIN as a constant for purposes of compile-time optimization. 
As of this writing there is no way for DATA_PIN to be a property that would vary from one object instance to another. 
So for now each string of leds needs its own controller, and an app can only use one instance of LedString. 

### Usage Example

```
#include "FastLED.h"

LedString lights;

void setup() {
  lights.doSetup("LLOFL SSFLO OOLSS FSSSL");
  lights.doStart();
} 

void loop() {
  lights.doLoop();
}
```

#### Explanation
**lights.doSetup** defines the LED behaviors.  
**lights.doStart** sets all the Lit and Off leds because they won't change, and turns the Switched leds on.  
**lights.doLoop**  changes the values of Fires, Switched leds and Customs, when it's time to do so.

## Custom Behavior
Custom behavior is a limited way to add more lighting options. If you write a void function that takes one int argument (the led number), and call **lights.setCustom**(yourFunction), your function will be called for every led whose behavior is specified with a "C". There are a few utility functions in LedString you can use within your function, such as turnOn(led) and turnOff(led), and you can reference lights.leds[n] for calling CRGB and CHSV functions. See the CustomBehavior example.  

## Notes
To control a 50-led WS2811 string, the wiring is straightforward: connect the LED string's control line to pin 3, connect an adequate 5V power supply to the LEDs, and tie its ground to the Arduino ground.  
I have powered a string of 8 leds cut off a 50-led string directly off an Arduino. 
