#include <WiFi.h>
#include <PubSubClient.h>
#include "crypto.h"
#include "protocol.h"

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "udb/dss101/g01t_seguro/canal";

WiFiClient espClient;
PubSubClient client(espClient);

CryptoEngine crypto;
NodeState currentState = STATE_IDLE;

size_t hexToBytes(const char* hexStr, uint8_t* byteArr, size_t maxLen) {
  size_t hexLen = strlen(hexStr);
  size_t byteCount = hexLen / 2;
  if (byteCount > maxLen) byteCount = maxLen;

  for (size_t i = 0; i < byteCount; i++) {
    char byteChars[3] = { hexStr[i * 2], hexStr[i * 2 + 1], '\0' };
    byteArr[i] = (uint8_t)strtoul(byteChars, NULL, 16);
  }
  return byteCount;
}

void callback(char* topic, byte* message, unsigned int length) {
  char hexFrame[length + 1];
  memcpy(hexFrame, message, length);
  hexFrame[length] = '\0';

  Serial.println("\n================================================");
  Serial.print("[RECEPTOR] Trama recibida (HEX): ");
  Serial.println(hexFrame);

  uint8_t rawBuffer[128];
  size_t rawLen = hexToBytes(hexFrame, rawBuffer, sizeof(rawBuffer));

  // Deserialización binaria (Integrante 2)
  Message incomingMsg;
  if (deserializeMessage(rawBuffer, rawLen, incomingMsg)) {
    Serial.printf("[RECEPTOR] Deserializado OK -> NodeID: %d | Tipo: %d | PSN: %d | Len: %d\n",
                  incomingMsg.nodeID, (int)incomingMsg.type, incomingMsg.psn, incomingMsg.payloadLen);

    // Procesa el mensaje a través de la máquina de estados (Integrante 2 y 1)
    processMessage(incomingMsg);
  } else {
    Serial.println("[RECEPTOR] Error: Deserialización fallida");
  }
}

void setup_wifi() {
  delay(10);
  Serial.print("\n[WIFI] Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WIFI] Conectado exitosamente.");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("[MQTT] Conectando...");
    String clientId = "ESP32-Rx-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println(" ¡Conectado!");
      client.subscribe(mqtt_topic);
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setKeepAlive(60);
  client.setCallback(callback);
  Serial.println("[RECEPTOR] En espera de mensaje FCM...");
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}