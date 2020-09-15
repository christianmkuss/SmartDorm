#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"

// Relay Pins
#define GREEN_PIN 14
#define RED_PIN 12
#define BLUE_PIN 5
#define PARTY_PIN 4

#define SERIAL_BAUDRATE 115200

// Wifi Passcode
#define WIFI_SSID "The Temple"
#define WIFI_PASS "TextCheny1"

#define LIGHT "living room light"
fauxmoESP fauxmo;

// Wi-Fi Connection
void wifiSetup() {
  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);

  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void setup() {
  // Init serial port and clean garbage
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println();

  // Wi-Fi connection
  wifiSetup();

  // LED
  pinMode(GREEN_PIN, OUTPUT);
  digitalWrite(GREEN_PIN, HIGH);

  pinMode(RED_PIN, OUTPUT);
  digitalWrite(RED_PIN, HIGH);

  pinMode(BLUE_PIN, OUTPUT);
  digitalWrite(BLUE_PIN, HIGH);

  pinMode(PARTY_PIN, OUTPUT);
  digitalWrite(PARTY_PIN, HIGH);

  // By default, fauxmoESP creates it's own webserver on the defined port
  // The TCP port must be 80 for gen3 devices (default is 1901)
  // This has to be done before the call to enable()
  fauxmo.createServer(true); // not needed, this is the default value
  fauxmo.setPort(80); // This is required for gen3 devices

  // You have to call enable(true) once you have a WiFi connection
  // You can enable or disable the library at any moment
  // Disabling it will prevent the devices from being discovered and switched
  fauxmo.enable(true);

  // Add virtual devices
  fauxmo.addDevice(LIGHT);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
    Serial.printf("Value: %d\n", value);
    Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

    // White Light (On 100%)
    if (state && value == 254) {
      Serial.println("Turning on white light");
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(RED_PIN, LOW);
      digitalWrite(BLUE_PIN, LOW);
      digitalWrite(PARTY_PIN, HIGH);
    }
    // Green Light (On 75%)
    else if (state && value == 191) {
      Serial.println("Turning on green light");
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(BLUE_PIN, HIGH);
      digitalWrite(PARTY_PIN, HIGH);
    }
    // Red (On 50%)
    else if (state && value == 128) {
      Serial.println("Turning on red light");
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(RED_PIN, LOW);
      digitalWrite(BLUE_PIN, HIGH);
      digitalWrite(PARTY_PIN, HIGH);
    }
    // Blue (On 25%)
    else if (state && value == 64) {
      Serial.println("Turning on blue light");
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(BLUE_PIN, LOW);
      digitalWrite(PARTY_PIN, HIGH);
    }
    // Party Light (On 1%)
    if (state && value == 4) {
      Serial.println("Turning on party light");
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(BLUE_PIN, HIGH);
      digitalWrite(PARTY_PIN, LOW);
     }
    // Off
    else {
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(BLUE_PIN, HIGH);
      digitalWrite(PARTY_PIN, HIGH);
    }
  });

}

void loop() {
  // fauxmoESP uses an async TCP server but a sync UDP server
  // Therefore, we have to manually poll for UDP packets
  fauxmo.handle();

  static unsigned long last = millis();
  if (millis() - last > 5000) {
    last = millis();
    Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
  }
}
