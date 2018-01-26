#include <LedString.h>

LedString lights;

void setup() {
  lights.doSetup("LLLFFFSS");
  delay(1000);
  lights.doStart();
}

void loop() {
  lights.doLoop();
}
