# LedString
FastLED wrapper to simplify lighting for model towns, castles, villages.

I've written a wrapper class using FastLED to light the buildings in a model town, medieval village, etc. 
This class is designed to use predefined semi-realistic behaviors to simplfy setting up and modifying lighting behavior. 
The class is not really meant for general animations.

The behavior of a string of LEDs is specified using a character string, one char per LED.  

#### Behaviors
L = always Lit (white)  
O = always Off (black) - in other words, not used  
S = Switched on and off at random to simulate human activity  
F = Flickering yellow light to simulate fireplaces, campfires, etc  
C = Custom

Example string: "LLOFL SSFLO OOLSS FSSSL"  
This example defines a string of 20 LEDs. The first two (LL) are constantly lit, the next one (O) is turned off (not being used), 
then there's a flickering fire (F) and another constantly lit one (L). 
Blanks can inserted anywhere for clarity; they are stripped out during operation.
  
The length of the text string is used to allocate the CRGB array dynamically. I know dynamic allocation is frowned upon 
because of the potential for memory fragmentation, but this step is only executed once in the sketch setup.

My goal was to control multiple strings of leds with a single Arduino or ESP8266, using a different DATA_PIN for each string,
to experiment and see how many leds a controller could reasonably handle. Unfortunately, FastLED requires specifying DATA_PIN as 
a constant for purposes of compile-time optimization. As of this writing there is no way for DATA_PIN to be a property that would
vary from one object instance to another. So for now each string of leds needs its own controller, and an app can only use one
instance of LedString. 

### Simple Example

```
#include "FastLED.h"

LedString lights;

void setup() {
  lights.begin("LLOFL SSFLO OOLSS FSSSL");
  lights.start();
} 

void loop() {
  lights.loop();
}
```

#### Explanation
**lights.begin** defines the LED behaviors as chars. Blanks can be inserted in the string for clarity; they get stripped out.  
**lights.start** sets the Lit or Off leds, because they won't change, and turns on the Switched ones.  
**lights.loop** waits until it's time to refresh the leds and makes the changes (flickering fires, random switching on and off, and Custom behavior).  

Custom behaviors are currently implemented as a hard-coded method in the class itself.  
To do: accept a pointer to a custom method.


