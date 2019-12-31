/*
    Start date: 23-10-2019
    Token: GSEsKtmqqwywGPCvl8JbHi2MUddEKJnN
*/

#define COUNT 2

#include <Blynk.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

char* auth = "GSEsKtmqqwywGPCvl8JbHi2MUddEKJnN";
char* ssid = "";  
char* pass = "";

int* outputLeg = new int[COUNT] {D6, D8};

/* ---------------------------------------------------------------- */
void connectWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.beginSmartConfig();

    Serial.println("Waiting for SmartConfig.");

    while (!WiFi.smartConfigDone()) {
        delay(1000);
        Serial.println("[SmartConfig] ...");
    }

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("[Wifi] ...");
    }

    Serial.println("[Wifi] Connected Successly");

    connectBlynk();
}

/* ---------------------------------------------------------------- */
void connectBlynk() {
    Blynk.config(auth);

    while (!Blynk.connected()) {
        Serial.println("[Blynk] ...");
        Blynk.connect();
    }

    Serial.println("[Blynk] Connected Successly");
}

/* ---------------------------------------------------------------- */
void setup() {
    Serial.begin(115200);

    for (int i = 0; i < COUNT; i++) {
        pinMode(outputLeg[i], OUTPUT);
    }

    connectWifi();
}

/* ---------------------------------------------------------------- */
void loop() {
    if (Blynk.connected()) {
        Blynk.run();
    }
}

/* ---------------------------------------------------------------- */
BLYNK_WRITE(V1) {
    int pinValue = param.asInt();
    digitalWrite(outputLeg[0], pinValue);
    Serial.print("[Blynk] V1 --> ");
    Serial.println(pinValue);
}

/* ---------------------------------------------------------------- */
BLYNK_WRITE(V2) {
    int pinValue = param.asInt();
    digitalWrite(outputLeg[1], pinValue);
    Serial.print("[Blynk] V2 --> ");
    Serial.println(pinValue);
}
