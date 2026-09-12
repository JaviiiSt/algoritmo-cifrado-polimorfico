#include <stdio.h>
#include <string.h>
#include "crypto.h"

int main() {
    CryptoEngine crypto;

    uint64_t masterKey = 0x123456789ABCDEF0ULL;
    crypto.generateKeyTables(masterKey);

    char message[] = "Mensaje Secreto ESP32";
    size_t len = strlen(message);
    uint32_t psn = 101; // Packet Sequence Number

    printf("=== NODO A (EMISOR) ===\n");
    printf("Mensaje original: %s\n", message);

    crypto.encryptPayload((uint8_t*)message, len, psn);

    printf("Payload cifrado enviado: ");
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", (uint8_t)message[i]);
    }
    printf("\n");

    return 0;
}