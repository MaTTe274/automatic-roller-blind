#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

#include <SinricPro.h>
#include <SinricProSwitch.h>



bool RELAYPIN_1; // switch avanti
bool RELAYPIN_2; // switch indietro
bool avanti;
bool indietro;

#define Pin1 1
#define Pin2 2
#define Pin3 3
#define Pin4 4

bool rotazione = false; // variabile per senso rotazione
int ritardo = 5; //velocita del motore



struct RelayInfo {
  String deviceId;
  String name;
  int pin;
};

//creare due dispositivi su sinric e incollare gli ID

std::vector<RelayInfo> relays = {
    {" ID device1 ", "Relay 1", RELAYPIN_1},
    {" ID device2 ", "Relay 2", RELAYPIN_2},
  };
  
#define WIFI_SSID         "      "  // Change WIFI_SSID to your WiFi Name.
#define WIFI_PASS         "      "  // Change WIFI_PASS to your WiFi password.
#define APP_KEY           "      "  // Paste App Key from above 
#define APP_SECRET        "      "  // Paste App Secret from above 


#define BAUD_RATE  115200              // Change baudrate to your need

bool onPowerState(const String &deviceId, bool &state) {
  for (auto &relay : relays) {
    if (deviceId == relay.deviceId) {
      Serial.printf("Device %s turned %s\r\n", relay.name.c_str(), state ? "on" : "off");
      digitalWrite(relay.pin, state);

      if (relay.deviceId == "ID device1" && state) { // If relay1 is turned on
        avanti=0;
        indietro=1; // Attiva la funzione "Avanti"
      } else if (relay.deviceId == "ID device2" && state) { // If relay2 is turned on
        avanti=1;
        indietro=0; // Attiva la funzione "Indietro"
      }
      else if(!state){
        avanti=0;
        indietro=0;
      }

      return true;
    }
  }
  return false;
}


void setupRelayPins() {
  for (auto &relay : relays) {    // for each relay configuration
    pinMode(relay.pin, OUTPUT);     // set pinMode to OUTPUT
  }
}

void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  #if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP); 
  #elif defined(ESP32)
    WiFi.setSleep(false); 
  #endif

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro() {
  for (auto &relay : relays) {                             // for each relay configuration
    SinricProSwitch &mySwitch = SinricPro[relay.deviceId];   // create a new device with deviceId from relay configuration
    mySwitch.onPowerState(onPowerState);                     // attach onPowerState callback to the new device
  }

  SinricPro.onConnected([]() { Serial.printf("Connected to SinricPro\r\n"); });
  SinricPro.onDisconnected([]() { Serial.printf("Disconnected from SinricPro\r\n"); });

  SinricPro.begin(APP_KEY, APP_SECRET);
}

void Avanti() {
  // Step 1
  digitalWrite(Pin1, HIGH);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, LOW);
  delay(ritardo);
   // Step 2
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, HIGH);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, LOW);
  delay(ritardo);
  // Step 3
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, HIGH);
  digitalWrite(Pin4, LOW);
  delay(ritardo);
  // Step 4
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, HIGH);
  delay(ritardo);
}

void Indietro() { 
  // Step 4
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, HIGH);
  delay(ritardo);
  // Step 3
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, HIGH);
  digitalWrite(Pin4, LOW);
  delay(ritardo);
  // Step 2
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, HIGH);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, LOW);
  delay(ritardo);
  // Step 1
  digitalWrite(Pin1, HIGH);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, LOW);
  delay(ritardo);
}

void setup() {
  Serial.begin(BAUD_RATE);
  setupRelayPins();
  setupWiFi();
  setupSinricPro();
  

  pinMode(Pin1, OUTPUT);
  pinMode(Pin2, OUTPUT);
  pinMode(Pin3, OUTPUT);
  pinMode(Pin4, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, LOW);
  digitalWrite(2, LOW);
}

void loop() {
  SinricPro.handle();
  if(avanti==1) 
  Avanti();

  if(indietro==1)
  Indietro();

}



