// Derived from https://gist.github.com/EEVblog/6206934
// and https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/examples/PostServer/PostServer.ino
// Built with PlatformIO, but should work on Arduino with a few minor changes.

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const unsigned int IR_LED_PIN = D1;
const unsigned int ONBOARD_LED = LED_BUILTIN;
const char* ssid = "ssid";
const char* password = "password";
const IPAddress IP(192, 168, 0, 66);
const IPAddress GATEWAY(192, 168, 0, 1);
const IPAddress SUBNET(255, 255, 255, 0);

ESP8266WebServer server(80);

void sendCarrierFrequency(const unsigned int timeMicroseconds) {
  for(unsigned int i = 0; i < (timeMicroseconds / 26); i++) {
    digitalWrite(IR_LED_PIN, HIGH);
    delayMicroseconds(11);
    digitalWrite(IR_LED_PIN, LOW); // 22
    delayMicroseconds(11);
  }
}

void sendCode(const uint8_t* data, const unsigned int start, const unsigned int length) {
  for(unsigned int i = 0; i < length; i++) {
    sendCarrierFrequency(710);
    if(data[start + i])
      delayMicroseconds(1500);
    else
      delayMicroseconds(500);
  }
  sendCarrierFrequency(500);
}

void sendState(const uint8_t* rawData) {
  digitalWrite(IR_LED_PIN, LOW);

  sendCarrierFrequency(9000);
  delayMicroseconds(4500);
  sendCode(rawData, 0, 35);

  digitalWrite(IR_LED_PIN, HIGH);
  delay(20);
  sendCode(rawData, 35, 32);

  digitalWrite(IR_LED_PIN, HIGH);
  delay(40);
  sendCarrierFrequency(9000);
  delayMicroseconds(4500);
  sendCode(rawData, 67, 35);

  digitalWrite(IR_LED_PIN, HIGH);
  delay(20);
  sendCode(rawData, 102, 32);

  digitalWrite(IR_LED_PIN, LOW);
}

void onSend() {
  if(server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else{
    server.send(200, "text/plain", "ok");

    String reqBody = server.arg("plain");
    uint8_t rawData[134];
    if(reqBody.length() != 134)
      return;
    for(unsigned int i = 0; i < reqBody.length(); ++i)
      if(reqBody.charAt(i) == '1')
        rawData[i] = 1;
      else
        rawData[i] = 0;
    sendState(rawData);
  }
}

void handleNotFound() {
  server.send(404, "text/plain", "404");
}

void setup(void) {
  pinMode(IR_LED_PIN, OUTPUT);
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, HIGH);

  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.config(IP, GATEWAY, SUBNET);
  WiFi.begin(ssid, password);

  // Wait for connection
  while(WiFi.status() != WL_CONNECTED)
    delay(500);

  server.on("/send/", std::function<void(void)>(onSend));
  server.onNotFound(handleNotFound);

  server.begin();
}

void loop(void) {
  server.handleClient();
}