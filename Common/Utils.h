#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <windows.h>


class Utils {
public:
    
    static std::string GetSystemInfo();

    
    static uint64_t GetTimestamp();

    
    static std::string Base64Encode(const std::vector<uint8_t>& data);

    
    static std::vector<uint8_t> Base64Decode(const std::string& encoded);

    
    static bool IsElevated();
};
