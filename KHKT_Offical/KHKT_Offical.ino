#include <Blynk.h>
#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

long delayTime = 5;
int countPin = 1;
boolean debouncing = false;
long currentTime = millis();

char* auth = "e6da2e85f72c40ae8409b0cbde81add9";
char* ssid = "";
char* pass = "";

int* stateLeg = new int[countPin] {LOW};
int* stateLegTemp = new int[countPin] {LOW};
int* inputLeg = new int[countPin] {04};
int* outputLeg = new int[countPin + 1] {14, 00};

Thread thread;

void setup() {
  for (int i = 0; i < countPin; i++) {
    pinMode(inputLeg[i], INPUT);
  }
  for (int i = 0; i < countPin + 1; i++) {
    pinMode(outputLeg[i], OUTPUT);
  }
  Serial.begin(9600);
  Blynk.config(auth);
  WiFi.mode(WIFI_STA);
  thread = Thread();
  thread.setInterval(3000);
  thread.onRun(connectWifi);
}

void updateStateTemp(int x, int value) {
  stateLegTemp[x] = value;
}

void updateState(int x) {
  stateLeg[x] = !stateLeg[x];
  digitalWrite(outputLeg[x], stateLeg[x]);
  digitalWrite(outputLeg[x + 1], stateLeg[x]);
  if (x == 0) {
    Blynk.virtualWrite(V3, stateLeg[0]);
  }
}

void changeState(int x) {
  int readState = digitalRead(inputLeg[x]);
  if (readState == HIGH) {
    if (millis() - currentTime >= delayTime) {
      currentTime = millis();
      if (stateLegTemp[x] != readState) {
        updateState(x);
        updateStateTemp(x, readState);
      }
    }
  } else {
    updateStateTemp(x, readState);
  }
}

void connectWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Waiting connect Wifi ...");
    WiFi.beginSmartConfig();
    delay(1000);
    if (WiFi.smartConfigDone()) {
      Serial.println("Wifi connected successly");
    }
  } else {
    if (!Blynk.connected()) {
      Serial.println("Waiting connect Blynk ...");
      Blynk.connect(1000);
    } else {
      Serial.println("Blynk connected successly");
      Blynk.virtualWrite(V3, stateLeg[0]);
      thread.enabled = false;
      thread.setInterval(-1);
    }
  }
}

void loop() {
  for (int i = 0; i < countPin; i++) {
    digitalWrite(outputLeg[i], stateLeg[i]);
    digitalWrite(outputLeg[i + 1], stateLeg[i]);
  }
  for (int i = 0; i < countPin; i++) {
    updateState(i);
  }
  if (thread.shouldRun()) thread.run();
  if (Blynk.connected()) {
    Blynk.run();
  }
}

BLYNK_WRITE(V3) {
  int pinValue = param.asInt();
  stateLeg[0] = pinValue;
  Serial.println('V3 -> ', pinValue);
}
