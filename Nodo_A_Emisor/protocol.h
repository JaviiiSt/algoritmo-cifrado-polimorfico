#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>

enum MessageType : uint8_t {
  MSG_FCM = 0x01,  //FCM
  MSG_RM  = 0x02,  //RM
  MSG_KUM = 0x03,  //KUM
  MSG_LCM = 0x04   //LCM
};

enum NodeState : uint8_t //Estados del nodo
  STATE_IDLE,       
  STATE_CONNECTED,  
  STATE_TERMINATED   
};

#define MAX_PAYLOAD_SIZE 32   

struct Message { //Estructura del mesnaje 
  uint8_t     nodeID;                    
  MessageType type;                       
  uint8_t     payload[MAX_PAYLOAD_SIZE];  
  uint8_t     payloadLen;                 
  uint8_t     psn;                       
};

// Convierte el mensaje a los bytes
uint8_t serializeMessage(const Message &msg, uint8_t *outBuffer, uint8_t bufferSize) {
  uint8_t totalLen = 4 + msg.payloadLen;
  if (bufferSize < totalLen) return 0;

  // Identificadores, tipos de mensajes, datos transmitidos, cantidad real de bytes y PSN
  outBuffer[0] = msg.nodeID;
  outBuffer[1] = (uint8_t)msg.type;
  outBuffer[2] = msg.psn & 0x0F;    
  outBuffer[3] = msg.payloadLen;

  for (uint8_t i = 0; i < msg.payloadLen; i++) {
    outBuffer[4 + i] = msg.payload[i];
  }
  return totalLen;
}

//Convierte el mensaje en bytes al original
bool deserializeMessage(const uint8_t *buffer, uint8_t bufferLen, Message &outMsg) {
  if (bufferLen < 4) return false;

  outMsg.nodeID     = buffer[0];
  outMsg.type       = (MessageType)buffer[1];
  outMsg.psn        = buffer[2] & 0x0F;
  outMsg.payloadLen = buffer[3];

  if (outMsg.payloadLen > MAX_PAYLOAD_SIZE) return false;
  if (bufferLen < (uint8_t)(4 + outMsg.payloadLen)) return false;

  for (uint8_t i = 0; i < outMsg.payloadLen; i++) {
    outMsg.payload[i] = buffer[4 + i];
  }
  return true;
}

//Modulo criptográfico
extern void   generateKeyTables(uint32_t P, uint32_t Q, uint32_t seed);
extern void   encryptPayload(uint8_t *payload, uint8_t len, uint8_t psn);
extern void   decryptPayload(uint8_t *payload, uint8_t len, uint8_t psn);

NodeState currentState = STATE_IDLE;

//Controladores de las 4 transiciones 

//FCM 
void handleFCM(const Message &msg) {
  Serial.print("[FCM] Primer contacto del nodo ");
  Serial.println(msg.nodeID);

  currentState = STATE_CONNECTED;
  Serial.println("[FCM] Tablas de claves listas. Estado -> CONNECTED");
}

//RM 
void handleRM(Message &msg) {
  if (currentState != STATE_CONNECTED) {
    Serial.println("[RM] Descartado: el nodo no está en estado CONNECTED");
    return;
  }

  decryptPayload(msg.payload, msg.payloadLen, msg.psn);

  Serial.print("[RM] Mensaje descifrado: ");
  for (uint8_t i = 0; i < msg.payloadLen; i++) {
    Serial.print((char)msg.payload[i]);
  }
  Serial.println();
}

//KUM
void handleKUM(const Message &msg) {
  Serial.println("[KUM] Solicitud de actualización de claves");
  Serial.println("[KUM] Tablas reconstruidas");
}

//LCM
void handleLCM(const Message &msg) {
  Serial.print("[LCM] Fin de comunicación con nodo ");
  Serial.println(msg.nodeID);
  currentState = STATE_TERMINATED;
}

//Finalizador 
void processMessage(Message &msg) {
  switch (msg.type) {
    case MSG_FCM: handleFCM(msg); break;
    case MSG_RM:  handleRM(msg);  break;
    case MSG_KUM: handleKUM(msg); break;
    case MSG_LCM: handleLCM(msg); break;
    default:
      Serial.println("[ERROR] Tipo de mensaje desconocido");
  }
}
//Recibe el mensaje ya deserializado y lo dirige al controlador correspondiente segun su tipo

#endif
