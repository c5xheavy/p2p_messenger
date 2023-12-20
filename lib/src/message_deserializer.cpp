#include "message_deserializer.h"
#include "payload_deserializer.h"
#include "login_hasher.h"

Message MessageDeserializer::MessageFromBuffer(char* buffer, std::size_t buffer_size) {
    Message message{};

    std::memcpy(&message.id, buffer, sizeof(message.id));
    buffer += sizeof(message.id);

    message.source_login_hash = std::string(buffer, LoginHasher::hash_size);
    buffer += LoginHasher::hash_size;

    message.destination_login_hash = std::string(buffer, LoginHasher::hash_size);
    buffer += LoginHasher::hash_size;

    std::uint16_t payload_size;
    std::memcpy(&payload_size, buffer, sizeof(payload_size));
    buffer += sizeof(payload_size);

    std::string str_payload(buffer, payload_size);
    buffer += payload_size;

    message.payload = PayloadDeserializer::PayloadFromJson(json::parse(str_payload).as_object());
    return message;
}
