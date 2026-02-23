#include "Crypto.h"
#include <algorithm>

std::vector<uint8_t> Crypto::key_;

void Crypto::SetKey(const std::string& key) {
    key_.assign(key.begin(), key.end());
    if (key_.empty()) {
        key_ = { 0xDE, 0xAD, 0xBE, 0xEF }; 
    }
}

std::vector<uint8_t> Crypto::Encrypt(const std::vector<uint8_t>& data) {
    if (key_.empty()) {
        SetKey("");
    }

    std::vector<uint8_t> encrypted(data.size());
    XorCrypt(data.data(), encrypted.data(), data.size());
    return encrypted;
}

std::vector<uint8_t> Crypto::Decrypt(const std::vector<uint8_t>& data) {
    return Encrypt(data); 
}

void Crypto::XorCrypt(const uint8_t* input, uint8_t* output, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        output[i] = input[i] ^ key_[i % key_.size()];
    }
}
