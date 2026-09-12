#include "crypto.h"
#include <stdio.h>

CryptoEngine::CryptoEngine() {
    for (int i = 0; i < 8; i++) {
        fs_table[i] = 0;
        fg_table[i] = 0;
        fm_table[i] = 0;
    }
}

uint8_t CryptoEngine::fr_rol(uint8_t byte, uint8_t shift) {
    shift %= 8;
    return (byte << shift) | (byte >> (8 - shift));
}

uint8_t CryptoEngine::fr_ror(uint8_t byte, uint8_t shift) {
    shift %= 8;
    return (byte >> shift) | (byte << (8 - shift));
}

uint8_t CryptoEngine::fr_xor(uint8_t byte, uint8_t key) {
    return byte ^ key;
}

// Generación dinámica de tablas de llaves de 64 bits
void CryptoEngine::generateKeyTables(uint64_t masterKey) {
    for (int i = 0; i < 8; i++) {
        fs_table[i] = masterKey ^ (0xA5A5A5A5A5A5A5A5ULL * (i + 1));
        fg_table[i] = (masterKey << (i + 1)) | (masterKey >> (64 - (i + 1)));
        fm_table[i] = fs_table[i] ^ fg_table[7 - i];
    }
}

// Banco de funciones reversibles fr()
uint8_t CryptoEngine::applyFR(uint8_t byte, uint8_t fn_index, uint64_t subkey) {
    uint8_t k = (uint8_t)(subkey & 0xFF);
    switch (fn_index % 3) {
        case 0: return fr_xor(byte, k);
        case 1: return fr_rol(byte, (k % 7) + 1);
        case 2: return fr_xor(fr_rol(byte, 3), k);
        default: return byte;
    }
}

// Inversa de las funciones reversibles
uint8_t CryptoEngine::applyFRInverse(uint8_t byte, uint8_t fn_index, uint64_t subkey) {
    uint8_t k = (uint8_t)(subkey & 0xFF);
    switch (fn_index % 3) {
        case 0: return fr_xor(byte, k);
        case 1: return fr_ror(byte, (k % 7) + 1);
        case 2: return fr_ror(fr_xor(byte, k), 3);
        default: return byte;
    }
}

// Cifrado polimórfico gobernado por PSN
void CryptoEngine::encryptPayload(uint8_t* payload, size_t length, uint32_t psn) {
    for (size_t i = 0; i < length; i++) {
        uint8_t keyIndex = (psn + i) % 8;
        uint8_t fnIndex = (psn + i) % 3; // depende solo de la posición, no del byte
        payload[i] = applyFR(payload[i], fnIndex, fs_table[keyIndex]);
    }
}

// Descifrado polimórfico gobernado por PSN
void CryptoEngine::decryptPayload(uint8_t* payload, size_t length, uint32_t psn) {
    for (size_t i = 0; i < length; i++) {
        uint8_t keyIndex = (psn + i) % 8;
        uint8_t fnIndex = (psn + i) % 3; // mismo cálculo que en el cifrado
        payload[i] = applyFRInverse(payload[i], fnIndex, fs_table[keyIndex]);
    }
}

// Impresión de tablas
void CryptoEngine::printKeyTables() {
    printf("--- TABLAS DE LLAVES (64 BITS) ---\n");
    for (int i = 0; i < 8; i++) {
        printf("fs[%d]: 0x%016llX | fg[%d]: 0x%016llX | fm[%d]: 0x%016llX\n",
               i, (unsigned long long)fs_table[i], 
               i, (unsigned long long)fg_table[i], 
               i, (unsigned long long)fm_table[i]);
    }
    printf("----------------------------------\n");
}