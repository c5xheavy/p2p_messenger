#include "message_serializer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include <opendht.h>

#include "message.h"
#include "payload_serializer.h"

namespace json = boost::json;

std::vector<uint8_t> MessageSerializer::message_to_buffer(const Message& message, std::shared_ptr<dht::crypto::PublicKey> public_key) {
    std::vector<uint8_t> payload_buffer{PayloadSerializer::payload_to_buffer(message.payload)};

    if (public_key) {
        payload_buffer = PayloadSerializer::encrypt(payload_buffer, *public_key);
    }

    std::size_t size{sizeof(message.id)
                     + sizeof(std::uint8_t) + message.source_login.size()
                     + sizeof(std::uint8_t) + message.destination_login.size()
                     + sizeof(std::uint16_t) + payload_buffer.size()};

    std::vector<uint8_t> buffer(size);
    uint8_t* ptr{buffer.data()};

    std::memcpy(ptr, &message.id, sizeof(message.id));
    ptr += sizeof(message.id);

    std::uint8_t source_login_size{static_cast<std::uint8_t>(message.source_login.size())};
    std::memcpy(ptr, &source_login_size, sizeof(source_login_size));
    ptr += sizeof(source_login_size);

    std::memcpy(ptr, message.source_login.data(), source_login_size);
    ptr += source_login_size;

    std::uint8_t destination_login_size{static_cast<std::uint8_t>(message.destination_login.size())};
    std::memcpy(ptr, &destination_login_size, sizeof(destination_login_size));
    ptr += sizeof(destination_login_size);

    std::memcpy(ptr, message.destination_login.data(), destination_login_size);
    ptr += destination_login_size;

    std::uint16_t payload_buffer_size{static_cast<std::uint16_t>(payload_buffer.size())};
    std::memcpy(ptr, &payload_buffer_size, sizeof(payload_buffer_size));
    ptr += sizeof(payload_buffer_size);

    std::memcpy(ptr, payload_buffer.data(), payload_buffer_size);
    ptr += payload_buffer_size;

    return buffer;
}
