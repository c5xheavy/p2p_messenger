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
                     + sizeof(uint8_t) + message.source_login.size()
                     + sizeof(uint16_t) + message.source_public_key.size()
                     + sizeof(uint8_t) + message.destination_login.size()
                     + sizeof(uint16_t) + payload_buffer.size()};

    std::vector<uint8_t> buffer(size);
    uint8_t* ptr{buffer.data()};

    std::memcpy(ptr, &message.id, sizeof(message.id));
    ptr += sizeof(message.id);

    uint8_t source_login_size{static_cast<uint8_t>(message.source_login.size())};
    std::memcpy(ptr, &source_login_size, sizeof(source_login_size));
    ptr += sizeof(source_login_size);

    std::memcpy(ptr, message.source_login.data(), source_login_size);
    ptr += source_login_size;

    uint16_t source_public_key_size{static_cast<uint16_t>(message.source_public_key.size())};
    std::memcpy(ptr, &source_public_key_size, sizeof(source_public_key_size));
    ptr += sizeof(source_public_key_size);

    std::memcpy(ptr, message.source_public_key.data(), source_public_key_size);
    ptr += source_public_key_size;

    uint8_t destination_login_size{static_cast<uint8_t>(message.destination_login.size())};
    std::memcpy(ptr, &destination_login_size, sizeof(destination_login_size));
    ptr += sizeof(destination_login_size);

    std::memcpy(ptr, message.destination_login.data(), destination_login_size);
    ptr += destination_login_size;

    uint16_t payload_buffer_size{static_cast<uint16_t>(payload_buffer.size())};
    std::memcpy(ptr, &payload_buffer_size, sizeof(payload_buffer_size));
    ptr += sizeof(payload_buffer_size);

    std::memcpy(ptr, payload_buffer.data(), payload_buffer_size);
    ptr += payload_buffer_size;

    return buffer;
}

std::vector<uint8_t> MessageSerializer::sign(const std::vector<uint8_t>& buffer, std::shared_ptr<dht::crypto::PrivateKey> private_key) {
    return private_key->sign(buffer);
}

std::vector<uint8_t> MessageSerializer::signed_message_to_buffer(const SignedMessage& signed_message) {
    std::size_t size{sizeof(uint16_t) + signed_message.message.size()
                     + sizeof(uint16_t) + signed_message.signature.size()};

    std::vector<uint8_t> buffer(size);
    uint8_t* ptr{buffer.data()};

    uint16_t message_size{static_cast<uint16_t>(signed_message.message.size())};
    std::memcpy(ptr, &message_size, sizeof(message_size));
    ptr += sizeof(message_size);

    std::memcpy(ptr, signed_message.message.data(), message_size);
    ptr += message_size;

    uint16_t signature_size{static_cast<uint16_t>(signed_message.signature.size())};
    std::memcpy(ptr, &signature_size, sizeof(signature_size));
    ptr += sizeof(signature_size);

    std::memcpy(ptr, signed_message.signature.data(), signature_size);
    ptr += signature_size;

    return buffer;
}
