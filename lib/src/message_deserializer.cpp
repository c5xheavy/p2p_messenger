#include "message_deserializer.h"
#include "payload_deserializer.h"

Message MessageDeserializer::MessageFromBuffer(char* buffer, std::size_t buffer_size) {
    Message message{};

    std::memcpy(&message.id, buffer, sizeof(message.id));
    buffer += sizeof(message.id);

    std::uint8_t source_login_size;
    std::memcpy(&source_login_size, buffer, sizeof(source_login_size));
    buffer += sizeof(source_login_size);

    message.source_login = std::string(buffer, source_login_size);
    buffer += source_login_size;

    std::uint8_t destination_login_size;
    std::memcpy(&destination_login_size, buffer, sizeof(destination_login_size));
    buffer += sizeof(destination_login_size);

    message.destination_login = std::string(buffer, destination_login_size);
    buffer += destination_login_size;

    std::uint16_t payload_size;
    std::memcpy(&payload_size, buffer, sizeof(payload_size));
    buffer += sizeof(payload_size);

    std::string strPayload(buffer, payload_size);
    buffer += payload_size;

    message.payload = PayloadDeserializer::PayloadFromJson(json::parse(strPayload).as_object());
    return message;
}
