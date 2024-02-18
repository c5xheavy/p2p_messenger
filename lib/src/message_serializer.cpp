#include "message_serializer.h"
#include "payload_serializer.h"
#include "login_hasher.h"

std::pair<std::shared_ptr<char[]>, std::size_t> MessageSerializer::MessageToBuffer(const Message& message) {
    std::string str_payload{json::serialize(PayloadSerializer::PayloadToJson(message.payload))};

    std::size_t size{sizeof(message.id)
                     + 2 * LoginHasher::hash_size
                     + sizeof(std::uint16_t) + str_payload.size()};

    const std::shared_ptr<char[]> buffer{new char[size]};
    char* ptr{buffer.get()};

    std::memcpy(ptr, &message.id, sizeof(message.id));
    ptr += sizeof(message.id);

    std::memcpy(ptr, message.source_login_hash.data(), message.source_login_hash.size());
    ptr += message.source_login_hash.size();

    std::memcpy(ptr, message.destination_login_hash.data(), message.destination_login_hash.size());
    ptr += message.destination_login_hash.size();

    std::uint16_t str_payload_size{static_cast<std::uint16_t>(str_payload.size())};
    std::memcpy(ptr, &str_payload_size, sizeof(str_payload_size));
    ptr += sizeof(str_payload_size);

    std::memcpy(ptr, str_payload.data(), str_payload_size);
    ptr += str_payload_size;

    return {buffer, size};
}
