#ifndef ENCRYPTING_H
#define ENCRYPTING_H

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <cstdint>

typedef struct {
    std::vector<uint8_t> decrypted_bytes;
    uint8_t key;
} result;

std::vector<uint8_t> fixed_xor(std::vector<uint8_t> start, std::vector<uint8_t> key);
int character_lookup(std::vector<uint8_t> byte_array, int key);
std::map<int, uint8_t> get_scores(std::vector<uint8_t> byte_array);
std::vector<uint8_t> single_byte_xor(std::vector<uint8_t> byte_array, uint8_t key);
result attack_single_byte_xor(std::vector<uint8_t> cipher_bytes);

#endif // ENCRYPTING_H