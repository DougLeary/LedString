//// LED settings
#include <LedString.h>
#define NUM_STRIPS 5
#define NUM_LEDS 50
#define DATA_PIN_0 2  // top shelf
#define DATA_PIN_1 3
#define DATA_PIN_2 4
#define DATA_PIN_3 5
#define DATA_PIN_4 6  // bottom shelf

CRGB leds[NUM_STRIPS][NUM_LEDS];
LedString strips[NUM_STRIPS];

String configFilename = "/config.txt";
struct Config {
  char pattern[NUM_STRIPS][NUM_LEDS];
};
Config config;

void setupLeds() {
  FastLED.clear();
  FastLED.addLeds<WS2811, DATA_PIN_0, RGB>(leds[0], NUM_LEDS);
  FastLED.addLeds<WS2811, DATA_PIN_1, RGB>(leds[1], NUM_LEDS);
  FastLED.addLeds<WS2811, DATA_PIN_2, RGB>(leds[2], NUM_LEDS);
  FastLED.addLeds<WS2811, DATA_PIN_3, RGB>(leds[3], NUM_LEDS);
  FastLED.addLeds<WS2811, DATA_PIN_4, RGB>(leds[4], NUM_LEDS);
  delay(100);
  for (int i=0; i<NUM_STRIPS; i++) {
    strips[i].doSetup(config.pattern[i], leds[i]);
  }
}

//// Stored config
#include <ArduinoJson.h>
#include <FS.h>

void getConfig() {
  Serial.println("Getting stored settings");
  String msg = "";
  if (!SPIFFS.begin()) {
    msg = "Error starting SPIFFS.";
  } else {
    File file = SPIFFS.open(configFilename, "r");
    if (!file) {
      msg = "Error opening settings file.";
    } else {
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, file);
      if (error) {
        msg = "Error reading settings.";
      } else {
        for (int i=0; i<NUM_STRIPS; i++) {
          String name = "pattern" + String(i);
          strlcpy(config.pattern[i], doc[name] | "O", sizeof(config.pattern[i]));
          Serial.println("Pattern " + String(i) + ": " + config.pattern[i]);
        }
        file.close();
        return;
      }
    }
  }
  Serial.println(msg);
  return;
}

void saveConfig() {
  SPIFFS.remove(configFilename);
  File file = SPIFFS.open(configFilename, "w");
  if (file) {
    StaticJsonDocument<512> doc;
    for (int i=0; i<NUM_STRIPS; i++) {
      strips[i].pattern.toCharArray(config.pattern[i], NUM_LEDS);
      String name = "pattern" + String(i);
      doc[name] = config.pattern[i];
    }
    if (serializeJson(doc, file) == 0) {
      Serial.println("Error saving config.");
    } else {
      Serial.println("Config saved.");
    }
    file.close();
  }
}

long customInterval = 1000L;            // set ms between toggle events
long lastCustomTime = -customInterval;  // make the first toggle happen immediately

void customBehavior(CRGB led) {
  long msNow = millis();
  // if customInterval has elapsed since the last toggle, it's time to toggle
  if (msNow - lastCustomTime > customInterval)
  {
    lastCustomTime = msNow;
    // if the led is red make it blue, else make it red
    if (led.red == 255) {
      led = CRGB::Blue;
    } else {
      led = CRGB::Red;
    }
  }
  FastLED.show();   // physically update the leds
}

//// Web server
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "netAccess.h"    // local credentials

const char* ssid     = NET_SSID;
const char* password = NET_PASSWORD;
ESP8266WebServer server(80);

const String formHtml = "\
<html>\
  <head>\
    <title>Xmas Town Lighting Control</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; \
        color: #000088; font-size: 24px;}\
      .pattern { width: 530px; }\
      .prompt { font-weight: bold; padding-right: 15px; }\
    </style>\
  </head>\
  <body>\
    <h1>Xmas Town Lighting Control</h1>\
    <span style=\"font-weight: bold;\">W: White, Y: Yellow, R: Red, G: Green, B: Blue, S: Switched, F: Fire</span>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\
      <span class=\"prompt\">Level 5</span><input class=\"pattern\" type=\"text\" name=\"pat0\" value=\"$0$\"><br/>\
      <span class=\"prompt\">Level 4</span><input class=\"pattern\" type=\"text\" name=\"pat1\" value=\"$1$\"><br/>\
      <span class=\"prompt\">Level 3</span><input class=\"pattern\" type=\"text\" name=\"pat2\" value=\"$2$\"><br/>\
      <span class=\"prompt\">Level 2</span><input class=\"pattern\" type=\"text\" name=\"pat3\" value=\"$3$\"><br/>\
      <span class=\"prompt\">Level 1</span><input class=\"pattern\" type=\"text\" name=\"pat4\" value=\"$4$\"><br/>\
      <input type=\"submit\" value=\"Apply\">\
    </form>\
  </body>\
</html>";

void handleRoot() {
  String st = formHtml;
  st.replace("$0$", strips[0].pattern);
  st.replace("$1$", strips[1].pattern);
  st.replace("$2$", strips[2].pattern);
  st.replace("$3$", strips[3].pattern);
  st.replace("$4$", strips[4].pattern);
  server.send(200, "text/html", st);
}

void handlePlain() {
  handleRoot();
}

void handleForm() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    Serial.println("**** UPDATING");
    for (int i=0; i<NUM_STRIPS; i++) {
      String st = server.arg(i);
      strips[i].setPattern(st);
    }
    saveConfig();
    handleRoot();
  }
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void startWebServer() {
  Serial.println("Connecting to WiFi");
  IPAddress staticIP(192, 168, 0, 197);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(8, 8, 8, 8);
  const char* deviceName = "xmastown";

  WiFi.disconnect();          // make sure there's no current connection
  WiFi.hostname(deviceName);  // DHCP hostname for finding static lease
  WiFi.mode(WIFI_STA);        // WiFi station, connects to router only
  WiFi.config(staticIP, subnet, gateway, dns);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/postplain/", handlePlain);
  server.on("/postform/", handleForm);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void setup() {
  Serial.begin(115200);
  Serial.println("");

  getConfig();
  
  Serial.println("Starting leds");
  setupLeds();
  startWebServer();
  Serial.println("Running");
}

void loop() {
  for (int i=0; i<NUM_STRIPS; i++) {
    strips[i].doLoop();
  }
  server.handleClient();
}
