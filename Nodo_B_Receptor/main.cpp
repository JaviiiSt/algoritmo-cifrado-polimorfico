#include <stdio.h>
#include <string.h>
#include "crypto.h"

int main() {
    CryptoEngine crypto;

    uint64_t masterKey = 0x123456789ABCDEF0ULL;
    crypto.generateKeyTables(masterKey);

      uint8_t encryptedMessage[] = {
      
    };
    size_t len = sizeof(encryptedMessage);
    uint32_t psn = 101; 

    printf("=== NODO B (RECEPTOR) ===\n");

    crypto.decryptPayload(encryptedMessage, len, psn);

    printf("Mensaje descifrado recuperado: %s\n", (char*)encryptedMessage);

    return 0;
}