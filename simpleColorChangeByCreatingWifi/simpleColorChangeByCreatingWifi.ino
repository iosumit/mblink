
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <Arduino_JSON.h>
#include <FastLED.h>

#define NUM_LEDS 1
#define DATA_PIN 16

CRGB leds[NUM_LEDS];

// Wifi name
const char *ssid = "mBlink";
const char *password = "mBlink123";
ESP8266WebServer server(80);

int red = 0;
int green = 0;
int blue = 220;

void setup() {
  Serial.begin(9600);
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Router IP address: ");
  Serial.println(myIP);
  route();
  Serial.println("HTTP server started");
  EEPROM.begin(756);
  red = EEPROM.read(0);//R
  green = EEPROM.read(1);//G
  blue = EEPROM.read(2);//B
  Serial.println("-------");
  Serial.println((String) "RED : "+red+", GREEN : "+green+", BLUE : "+blue);
  Serial.println("-------");
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  leds[0] = CRGB(red,green,blue);
  FastLED.show();
}

void loop() {
  server.handleClient();
}
void route(){
   server.enableCORS(true);
   server.on("/",root);
   server.on("/changecolor",  changecolor);
   server.on("/savechanges",  savechanges);
   server.onNotFound(notFound);
   server.begin();
}
void root(){
   server.send(200, "application/json", "{\"status\": true, \"message\":\"You are connected\"}");
}
void changecolor(){
  if (server.method() != HTTP_POST) {
    server.send(400, "application/json", "{\"status\": false, \"message\":\"Method not allowed\"}");
  } else if(server.hasArg("plain")) {
    String body;
    String headr;
    body = server.arg("plain");
    seriallizeChange(body);
    server.send(200, "application/json", "{\"status\": true, \"message\":\"Changed\"}");
  } else {
    server.send(500, "application/json", "{\"status\": false, \"message\":\"Bad request\"}");
  }
}
void savechanges(){
  if (server.method() != HTTP_POST) {
    server.send(400, "application/json", "{\"status\": false, \"message\":\"Method not allowed\"}");
  }
  Serial.println((String) "RED : "+red+", GREEN : "+green+", BLUE : "+blue);
  EEPROM.write(0, red);
  EEPROM.write(1, green);
  EEPROM.write(2, blue);
  boolean res = EEPROM.commit();
  Serial.println(res);
  String message = "";
  if(res){
    message = "Saved Successfully";
  }else {
    message = "Not saved, May be memory location not working";
  }
  delay(1000);
  server.send(200, "application/json", "{\"status\": true, \"message\":\""+message+"\"}");
}

void seriallizeChange(String& body){
  JSONVar doc = JSON.parse(body);
  red = doc["r"];
  green = doc["g"];
  blue = doc["b"];
  Serial.println(red);
  Serial.println(green);
  Serial.println(blue);
  leds[0] = CRGB(red,green,blue);
  FastLED.show();
}
void notFound(){
  server.send(404, "application/json", "{\"status\": false, \"message\":\"Not found\"}");
}
