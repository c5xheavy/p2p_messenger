#include "message_deserializer.h"
#include "payload_deserializer.h"

Message MessageDeserializer::MessageFromBuffer(const Buffer& buffer) {
    Message message{};
    std::size_t i = 0;
    for (std::size_t j = 0; j < 8; ++j) {
        message.id |= static_cast<std::uint64_t>(buffer[i]) << (j * 8);
        ++i;
    }
    std::uint8_t source_login_size = buffer[i++];
    for (std::uint8_t j = 0; j < source_login_size; ++j) {
        message.source_login.push_back(buffer[i++]);
    }
    std::uint8_t destination_login_size = buffer[i++];
    for (std::uint8_t j = 0; j < destination_login_size; ++j) {
        message.destination_login.push_back(buffer[i++]);
    }
    std::uint16_t payload_size = 0;
    for (std::size_t j = 0; j < 2; ++j) {
        payload_size |= static_cast<std::uint16_t>(buffer[i]) << (j * 8);
        ++i;
    }
    std::string strPayload;
    for (std::uint16_t j = 0; j < payload_size; ++j) {
        strPayload.push_back(buffer[i++]);
    }
    message.payload = PayloadDeserializer::PayloadFromJson(json::parse(strPayload).as_object());
    return message;
}
