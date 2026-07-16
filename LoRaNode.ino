// ============================================================
// LoRaNode.ino - Código principal 
// ============================================================
// Inclui as configurações de rádio e do nó específico
#include "lora_config.h"
#include "node_config.h"    

#include <SPI.h>
#include <LoRa.h>

// ==================== VERIFICAÇÕES ====================
#ifndef LOCAL_ADDRESS
  #error "Defina LOCAL_ADDRESS no arquivo de configuração do nó!"
#endif
#ifndef INTERVAL
  #error "Defina INTERVAL no arquivo de configuração do nó!"
#endif
#ifndef CARIMBO
  #error "Defina CARIMBO no arquivo de configuração do nó!"
#endif

// ==================== VARIÁVEIS GLOBAIS ====================
unsigned long LAST_SEND_TIME = 0;
unsigned long LAST_SEND_TIME_FORWARD_GO = 0;
unsigned long LAST_SEND_TIME_FORWARD_RETURN = 0;

String carimbo = CARIMBO;

bool flagForwardGo = false;
bool flagForwardReturn = false;
bool flagEnvio = false;

int bufferCont = -1;
byte buferDestinationAddress = 0x00;
byte bufferLocalAddress = 0x00;
byte bufferOriginAddress = 0x00;
String bufferMensagem = "";

int MsgCont = -1;

int ultimoContGo = -1;
byte ultimoOriginAddressGo = 0x00;

 

// Protótipos
void receiveMessage(int packetSize);
void sendMessage(String outgoing);
void forwardData(String outgoing);

// ==================== SETUP ====================
void setup() {
  Serial.begin(9600);
  Serial.println("Lora barramento: no 0x" + String(LOCAL_ADDRESS, HEX));

  LoRa.setPins(CS_PIN, RESET_PIN, IRQ_PIN);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("Falha na inicialização do LoRa. Verifique suas conexões.");
    while (true) {}
  }
  LoRa.setTxPower(LORA_POWER);
  LoRa.setSpreadingFactor(LORA_SF);
  LoRa.setCodingRate4(LORA_CR);

  Serial.print("Carimbo: ");
  Serial.print(carimbo);
  Serial.print(" Forward ida: ");
  Serial.print(INTERVAL_FORWARD_GO);
  Serial.print(" Forward volta: ");
  Serial.print(INTERVAL_FORWARD_RETURN);
  Serial.print(" Intervalo de Envio: ");
  Serial.println(INTERVAL);

  LAST_SEND_TIME = millis();
}

// ==================== LOOP ====================
void loop() {
  receiveMessage(LoRa.parsePacket());

  // Forward de requisição (ida)
  if (flagForwardGo) {
    if (millis() - LAST_SEND_TIME_FORWARD_GO > INTERVAL_FORWARD_GO) {
      Serial.println("Mensagem de REQUISIÇÃO liberada para encaminhar");
      forwardData(bufferMensagem);
      // Zera buffer
      bufferCont = -1;
      buferDestinationAddress = 0x00;
      bufferLocalAddress = 0x00;
      bufferOriginAddress = 0x00;
      bufferMensagem = "";
      flagForwardGo = false;
    }
  }

  // Forward de resposta (volta)
  if (flagForwardReturn) {
    if (millis() - LAST_SEND_TIME_FORWARD_RETURN > INTERVAL_FORWARD_RETURN) {
      Serial.println("Mensagem de ENVIO liberada para encaminhar");
      forwardData(bufferMensagem);
      // Zera buffer
      bufferCont = -1;
      buferDestinationAddress = 0x00;
      bufferLocalAddress = 0x00;
      bufferOriginAddress = 0x00;
      bufferMensagem = "";
      flagForwardReturn = false;
    }
  }

  // Envio periódico (resposta ao broadcast)
  if (flagEnvio) {
    if (millis() - LAST_SEND_TIME > INTERVAL) {
      sendMessage(carimbo);
      flagEnvio = false;
    }
  }
}

// ==================== RECEBER MENSAGEM ====================
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
    Serial.println("Erro: O comprimento da mensagem não corresponde ao comprimento");
    return;
  }

  Serial.print("Recebi de 0x" + String(sender, HEX));
  Serial.print(" orig em 0x" + String(origin, HEX));
  Serial.print(" msg ");
  Serial.println(msgNum);

  // --- Mensagem de ida (broadcast) ---
  if (origin == BROADCAST) {
    if (msgNum != ultimoContGo || ultimoOriginAddressGo != origin) {
      Serial.print("Mensagem de requisição: Preparando coleta e envio... msg: ");
      Serial.print(msgNum);
      Serial.print(" de 0x" + String(sender, HEX));
      Serial.print(" originada em 0x" + String(origin, HEX));
      Serial.print(" para 0x" + String(destination, HEX));
      Serial.println(" Caminho >> " + incoming);

      flagEnvio = true;
      LAST_SEND_TIME = millis();

      if (bufferCont == -1) {
        bufferCont = msgNum;
        buferDestinationAddress = destination;
        bufferLocalAddress = sender;
        bufferOriginAddress = origin;
        bufferMensagem = incoming + carimbo;

        MsgCont = msgNum;

        ultimoContGo = bufferCont;
        ultimoOriginAddressGo = bufferOriginAddress;

        Serial.print("Armazenando em Buffer!! Mensagem ID: ");
        Serial.print(bufferCont);
        Serial.print(" de 0x" + String(sender, HEX));
        Serial.print(" originada em 0x" + String(origin, HEX));
        Serial.print(" para 0x" + String(destination, HEX));
        Serial.print(" teve o caminho: ");
        Serial.println(bufferMensagem);
        Serial.println("################################################");

        flagForwardGo = true;
        LAST_SEND_TIME_FORWARD_GO = millis();
      }
    }
  }

  // --- Mensagem de volta (resposta) ---
  if (origin > LOCAL_ADDRESS && sender > LOCAL_ADDRESS && origin != BROADCAST) {
    if (origin == BROADCAST) {
      Serial.println("DESCARTANDO MENSAGEM DO NÓ ORIGEM");
      return;
    }

    Serial.print("Mensagem de ENVIO: ");
    Serial.print(msgNum);
    Serial.print(" de 0x" + String(sender, HEX));
    Serial.print(" originada em 0x" + String(origin, HEX));
    Serial.print(" para 0x" + String(destination, HEX));
    Serial.println(" Caminho >> " + incoming);

    if (bufferCont == -1) {
      bufferCont = msgNum;
      buferDestinationAddress = destination;
      bufferLocalAddress = sender;
      bufferOriginAddress = origin;
      bufferMensagem = incoming + carimbo;

      Serial.print("Armazenando em Buffer!! Mensagem ID: ");
      Serial.print(bufferCont);
      Serial.print(" de 0x" + String(sender, HEX));
      Serial.print(" originada em 0x" + String(origin, HEX));
      Serial.print(" para 0x" + String(destination, HEX));
      Serial.print(" teve o caminho: ");
      Serial.println(bufferMensagem);
      Serial.println("################################################");

      flagForwardReturn = true;
      LAST_SEND_TIME_FORWARD_RETURN = millis();
    }
  }

  // --- Mensagem de volta vinda de um nó mais próximo (para zerar buffer) ---
  if (origin > LOCAL_ADDRESS && sender < LOCAL_ADDRESS && origin != BROADCAST) {
    Serial.print("Mensagem de ENVIO de um nó mais proximo ");
    Serial.print(msgNum);
    Serial.print(" de 0x" + String(sender, HEX));
    Serial.print(" originada em 0x" + String(origin, HEX));
    Serial.print(" para 0x" + String(destination, HEX));
    Serial.println(" Caminho >> " + incoming);

    if (msgNum == bufferCont && origin == bufferOriginAddress) {
      flagForwardReturn = false;
      bufferCont = -1;
      buferDestinationAddress = 0x00;
      bufferLocalAddress = 0x00;
      bufferOriginAddress = 0x00;
      bufferMensagem = "";

      Serial.print("Zerando Buffer!! Mensagem ID: ");
      Serial.print(msgNum);
      Serial.print(" originada em 0x" + String(origin, HEX));
      Serial.print(" para 0x" + String(destination, HEX));
      Serial.print(" já foi encaminhada por");
      Serial.println(" 0x" + String(sender, HEX));
    }
    Serial.println("################################################");
  }
}

// ==================== ENVIAR MENSAGEM (resposta) ====================
void sendMessage(String outgoing) {
  byte originAddress = LOCAL_ADDRESS;

  Serial.print("Enviando: ");
  Serial.print(MsgCont);
  Serial.print(" de 0x" + String(LOCAL_ADDRESS, HEX));
  Serial.print(" para 0x" + String(BROADCAST, HEX));
  Serial.println(" | Inicia com nó: " + outgoing);
  Serial.println("################################################");

  LoRa.beginPacket();
  LoRa.write(MsgCont);
  LoRa.write(originAddress);
  LoRa.write(LOCAL_ADDRESS);
  LoRa.write(BROADCAST);
  LoRa.write(outgoing.length());
  LoRa.print(outgoing);
  LoRa.endPacket();
}

// ==================== ENCAMINHAR (forward) ====================
void forwardData(String outgoing) {
  LoRa.beginPacket();

  LoRa.write(bufferCont);
  LoRa.write(bufferOriginAddress);
  LoRa.write(LOCAL_ADDRESS);
  LoRa.write(buferDestinationAddress);
  LoRa.write(outgoing.length());
  LoRa.print(outgoing);
  LoRa.endPacket();

  Serial.print("Repassando: Mensagem ID: ");
  Serial.print(bufferCont);
  Serial.print(" de 0x" + String(LOCAL_ADDRESS, HEX));
  Serial.print(" para 0x" + String(BROADCAST, HEX));
  Serial.print(" originado em 0x" + String(bufferOriginAddress, HEX));
  Serial.println(" Caminho >> " + outgoing);
  Serial.println("################################################");
}
