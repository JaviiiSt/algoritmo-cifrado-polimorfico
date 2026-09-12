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
uint64_t masterKey = 0x123456789ABCDEF0ULL;

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
    String clientId = "ESP32-Tx-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println(" ¡Conectado!");
    } else {
      delay(2000);
    }
  }
}

void bytesToHex(const uint8_t* in, size_t len, char* out) {
  for (size_t i = 0; i < len; i++) {
    sprintf(out + (i * 2), "%02X", in[i]);
  }
  out[len * 2] = '\0';
}

void enviarMensajeProtocolo(MessageType type, const char* texto, uint8_t psn) {
  Message msg;
  msg.nodeID = 1;
  msg.type = type;
  msg.psn = psn & 0x0F;
  msg.payloadLen = strlen(texto);
  memcpy(msg.payload, texto, msg.payloadLen);

  Serial.println("\n================================================");
  Serial.print("[EMISOR] Preparando Tipo: ");
  Serial.print((int)type);
  Serial.print(" | Payload en Claro: ");
  Serial.println(texto);

  // Si es RM o KUM, se cifra polimórficamente el payload antes de serializar
  if (type == MSG_RM || type == MSG_KUM) {
    crypto.encryptPayload(msg.payload, msg.payloadLen, msg.psn);
    Serial.print("[EMISOR] Payload Cifrado: ");
    for (int i = 0; i < msg.payloadLen; i++) Serial.printf("%02X ", msg.payload[i]);
    Serial.println();
  }

  // Serialización con el módulo del Integrante 2
  uint8_t buffer[128];
  uint8_t bytesSerializados = serializeMessage(msg, buffer, sizeof(buffer));

  // Convertir a HEX para transmisión MQTT
  char hexFrame[256];
  bytesToHex(buffer, bytesSerializados, hexFrame);

  Serial.print("[EMISOR] Trama binaria serializada enviada (HEX): ");
  Serial.println(hexFrame);

  client.publish(mqtt_topic, hexFrame);
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setKeepAlive(60);

  crypto.generateKeyTables(masterKey);
  crypto.printKeyTables();
}

unsigned long lastMsg = 0;
int currentStep = 0;

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 4500) {
    lastMsg = now;

    switch (currentStep) {
    case 0:
        // FCM: Primer contacto (Restablecer tablas a la llave inicial)
        crypto.generateKeyTables(masterKey); 
        enviarMensajeProtocolo(MSG_FCM, "INIT_PQS_SEED", 0x05);
        currentStep++;
        break;
      case 1:
        // RM: Mensaje normal con payload cifrado
        enviarMensajeProtocolo(MSG_RM, "Sensor_Temp:26C", 0x07);
        currentStep++;
        break;
      case 2:
        // KUM: Actualización de llaves
        enviarMensajeProtocolo(MSG_KUM, "UPDATE_KEY_TABLE", 0x0A);
        crypto.generateKeyTables(0xFEDCBA9876543210ULL); // Emisor actualiza también
        currentStep++;
        break;
      case 3:
        // LCM: Fin de transmisión
        enviarMensajeProtocolo(MSG_LCM, "CLOSE_SESSION", 0x01);
        Serial.println("--- Ciclo finalizado. Reiniciando en 8 segundos... ---\n");
        currentStep = 0;
        lastMsg = now + 8000;
        break;
    }
  }
}