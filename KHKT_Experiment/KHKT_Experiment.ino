#include <Blynk.h>
#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

long delayTime = 5;
int countPin = 2;
boolean debouncing = false;
long currentTime = millis();

char* auth = "e6da2e85f72c40ae8409b0cbde81add9";
char* ssid = "";
char* pass = "";

int* stateLeg = new int[countPin] {LOW, LOW};
int* stateLegTemp = new int[countPin] {LOW, LOW};
int* inputLeg = new int[countPin] {14, 12};
int* outputLeg = new int[countPin] {05, 04};

Thread thread;

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
      Blynk.virtualWrite(V1, stateLeg[0]);
      Blynk.virtualWrite(V2, stateLeg[1]);
      thread.enabled = false;
      thread.setInterval(-1);
    }
  }
}

void updateStateTemp(int x, int value) {
  stateLegTemp[x] = value;
}

void updateState(int x) {
  stateLeg[x] = !stateLeg[x];
  digitalWrite(outputLeg[x], stateLeg[x]);
  if (x == 0) {
    Blynk.virtualWrite(V1, stateLeg[0]);
  }
  if (x == 1) {
    Blynk.virtualWrite(V2, stateLeg[1]);
  }
}

boolean changeState(int x) {
  // Thay đổi trạng thái nút mỗi lần bấm vào ( Khi bấm High ) là đổi
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
  return false;
}

void setup() {
  for (int i = 0; i < countPin; i++) {
    pinMode(inputLeg[i], INPUT);
  }
  for (int i = 0; i < countPin; i++) {
    pinMode(outputLeg[i], OUTPUT);
  }
  Serial.begin(9600);
  Blynk.config(auth);
  WiFi.mode(WIFI_STA);
  thread = Thread();
  thread.setInterval(3000);
  thread.onRun(connectWifi);
}

void loop() {
  for (int i = 0; i < countPin; i++) {
    digitalWrite(outputLeg[i], stateLeg[i]);
  }
  for (int i = 0; i < countPin; i++) {
    if (changeState(i)) {
      updateState(i);
    }
  }
  if (thread.shouldRun()) thread.run();
  if (Blynk.connected()) {
    Blynk.run();
  }
}


BLYNK_WRITE(V1) {
  int pinValue = param.asInt();
  stateLeg[0] = pinValue;
  Serial.println('V1 -> ', pinValue);
}

BLYNK_WRITE(V2) {
  int pinValue = param.asInt();
  stateLeg[1] = pinValue;
  Serial.println('V2 -> ', pinValue);
}
