#include "Protocol.h"
#include <cstring>
#include <stdexcept>

std::vector<uint8_t> Protocol::CreateMessage(MessageType type, const std::vector<uint8_t>& data, bool encrypt) {
    MessageHeader header;
    header.magic = MAGIC_NUMBER;
    header.length = static_cast<uint32_t>(data.size());
    header.type = type;
    header.encrypted = encrypt ? 1 : 0;
    header.checksum = CalculateChecksum(data.data(), data.size());

    std::vector<uint8_t> message;
    message.resize(sizeof(MessageHeader) + data.size());

    memcpy(message.data(), &header, sizeof(MessageHeader));
    if (!data.empty()) {
        memcpy(message.data() + sizeof(MessageHeader), data.data(), data.size());
    }

    return message;
}

bool Protocol::ParseMessage(const std::vector<uint8_t>& buffer, MessageHeader& header, std::vector<uint8_t>& data) {
    if (buffer.size() < sizeof(MessageHeader)) {
        return false;
    }

    memcpy(&header, buffer.data(), sizeof(MessageHeader));

    if (header.magic != MAGIC_NUMBER) {
        return false;
    }

    if (header.length > MAX_MESSAGE_SIZE) {
        return false;
    }

    if (buffer.size() < sizeof(MessageHeader) + header.length) {
        return false;
    }

    data.resize(header.length);
    if (header.length > 0) {
        memcpy(data.data(), buffer.data() + sizeof(MessageHeader), header.length);

        uint16_t checksum = CalculateChecksum(data.data(), data.size());
        if (checksum != header.checksum) {
            return false;
        }
    }

    return true;
}

uint16_t Protocol::CalculateChecksum(const uint8_t* data, size_t length) {
    uint32_t sum = 0;
    for (size_t i = 0; i < length; ++i) {
        sum += data[i];
    }
    return static_cast<uint16_t>(sum & 0xFFFF);
}

void Protocol::SerializeString(std::vector<uint8_t>& buffer, const std::string& str) {
    uint32_t length = static_cast<uint32_t>(str.length());
    size_t offset = buffer.size();
    buffer.resize(offset + sizeof(uint32_t) + length);

    memcpy(buffer.data() + offset, &length, sizeof(uint32_t));
    if (length > 0) {
        memcpy(buffer.data() + offset + sizeof(uint32_t), str.data(), length);
    }
}

std::string Protocol::DeserializeString(const uint8_t*& ptr, size_t& remaining) {
    if (remaining < sizeof(uint32_t)) {
        throw std::runtime_error("Buffer too small for string length");
    }

    uint32_t length;
    memcpy(&length, ptr, sizeof(uint32_t));
    ptr += sizeof(uint32_t);
    remaining -= sizeof(uint32_t);

    if (remaining < length) {
        throw std::runtime_error("Buffer too small for string data");
    }

    std::string result(reinterpret_cast<const char*>(ptr), length);
    ptr += length;
    remaining -= length;

    return result;
}
