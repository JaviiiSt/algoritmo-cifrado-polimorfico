#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>
#include <stddef.h>

class CryptoEngine {
private:
    uint64_t fs_table[8];
    uint64_t fg_table[8];
    uint64_t fm_table[8];

    uint8_t fr_xor(uint8_t byte, uint8_t key);
    uint8_t fr_rol(uint8_t byte, uint8_t shift);
    uint8_t fr_ror(uint8_t byte, uint8_t shift);

public:
    CryptoEngine();

    // 1. Generación de tablas de llaves de 64 bits (fs, fg, fm)
    void generateKeyTables(uint64_t masterKey);

    // 2. Banco de funciones reversibles fr() y sus inversas
    uint8_t applyFR(uint8_t byte, uint8_t fn_index, uint64_t subkey);
    uint8_t applyFRInverse(uint8_t byte, uint8_t fn_index, uint64_t subkey);

    // 3. Selector polimórfico gobernado por el PSN (cifrar y descifrar)
    void encryptPayload(uint8_t* payload, size_t length, uint32_t psn);
    void decryptPayload(uint8_t* payload, size_t length, uint32_t psn);

    // 4. Imprimir tablas de llaves (para la demostración en el video)
    void printKeyTables();
};

#endif