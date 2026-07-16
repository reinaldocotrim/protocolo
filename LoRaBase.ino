#include "lora_config.h"
#include "base_config.h"

#include <SPI.h>
#include <LoRa.h>
#include <SD.h>
#include "RTClib.h"

RTC_DS1307 rtc;
File dataFile;
#define CHIP_SELECT 10

unsigned long LAST_SEND_TIME = 0;
int msgNumber = 0;

void imprimeTempo();
void receiveMessage(int packetSize);
void sendMessage(String outgoing);

void setup() {
  Serial.begin(9600);
  SPI.begin();

  LoRa.setPins(CS_PIN, RESET_PIN, IRQ_PIN);
  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("Fail LoRa.");
    while (true) {}
  }
  LoRa.setTxPower(LORA_POWER);
  LoRa.setSpreadingFactor(LORA_SF);
  LoRa.setCodingRate4(LORA_CR);

  Serial.println("Nod 0x" + String(LOCAL_ADDRESS, HEX));
  Serial.print("Init SD ...");
  pinMode(CHIP_SELECT, OUTPUT);
  if (!SD.begin(CHIP_SELECT)) {
    Serial.println("Card fail");
    while (1) ;
  }
  Serial.println("card init.");

  dataFile = SD.open("campo.txt", FILE_WRITE);
  if (!dataFile) {
    Serial.println("error log");
    while (1) ;
  }

  if (!rtc.begin()) {
    Serial.println("RTCfail");
    Serial.flush();
    abort();
  }

  dataFile.print("On: ");
  dataFile.println(" ");
  Serial.print("On: ");
  imprimeTempo();

  Serial.print(" Stamp: ");
  Serial.print(CARIMBO);
  Serial.print(" gap: ");
  Serial.println(INTERVAL);

  LAST_SEND_TIME = millis();
}

void loop() {
  receiveMessage(LoRa.parsePacket());
  if (millis() - LAST_SEND_TIME > INTERVAL) {
    sendMessage(CARIMBO);
    LAST_SEND_TIME = millis();
  }
}

void receiveMessage(int packetSize) {
  if (packetSize == 0) return;

  int msgNum = LoRa.read();
  byte origin = LoRa.read();
  byte sender = LoRa.read();
  byte destination = LoRa.read();
  byte incomingLength = LoRa.read();
  String incoming = "";
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {
    Serial.println("Error msg");
    dataFile.println("Error msg");
    return;
  }

  imprimeTempo();
  Serial.print(" Msg ID: ");
  Serial.print(msgNum);
  Serial.print(" in 0x" + String(sender, HEX));
  Serial.print(" orig em 0x" + String(origin, HEX));
  Serial.print(" to 0x" + String(destination, HEX));
  Serial.print(" path-> ");
  Serial.println(incoming);

  dataFile.print(" Msg ID: ");
  dataFile.print(msgNum);
  dataFile.print(" in 0x" + String(sender, HEX));
  dataFile.print(" orig em 0x" + String(origin, HEX));
  dataFile.print(" to 0x" + String(destination, HEX));
  dataFile.print(" path-> ");
  dataFile.println(incoming);
  dataFile.flush();
}

void sendMessage(String outgoing) {
  imprimeTempo();
  byte originAddress = LOCAL_ADDRESS;

  Serial.print(" Send: ");
  Serial.print(msgNumber);
  Serial.print(" in 0x" + String(LOCAL_ADDRESS, HEX));
  Serial.print(" to 0x" + String(NETWORK_ADDRESS, HEX));
  Serial.println(" | Init in: " + outgoing);

  dataFile.print(" Send: ");
  dataFile.print(msgNumber);
  dataFile.print(" in 0x" + String(LOCAL_ADDRESS, HEX));
  dataFile.print(" to 0x" + String(NETWORK_ADDRESS, HEX));
  dataFile.println(" | Init in: " + outgoing);
  dataFile.flush();

  LoRa.beginPacket();
  LoRa.write(msgNumber);
  LoRa.write(originAddress);
  LoRa.write(LOCAL_ADDRESS);
  LoRa.write(NETWORK_ADDRESS);
  LoRa.write(outgoing.length());
  LoRa.print(outgoing);
  LoRa.endPacket();
  msgNumber++;
}

void imprimeTempo() {
  DateTime now = rtc.now();
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print('|');

  dataFile.print(now.hour(), DEC);
  dataFile.print(':');
  dataFile.print(now.minute(), DEC);
  dataFile.print(':');
  dataFile.print(now.second(), DEC);
  dataFile.print('|');
  dataFile.flush();
}
