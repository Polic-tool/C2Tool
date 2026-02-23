#pragma once
#include <vector>
#include <string>
#include <cstdint>


class Crypto {
public:
    
    static void SetKey(const std::string& key);

    
    static std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& data);

    
    static std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& data);

private:
    static std::vector<uint8_t> key_;
    static void XorCrypt(const uint8_t* input, uint8_t* output, size_t length);
};
