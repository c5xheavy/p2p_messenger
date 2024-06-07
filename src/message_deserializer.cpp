#include "message_deserializer.h"

#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <opendht.h>

#include "message.h"
#include "payload_deserializer.h"

namespace json = boost::json;

SignedMessage MessageDeserializer::signed_message_from_buffer(const std::vector<uint8_t>& buffer) {
    SignedMessage signed_message{};

    size_t size{sizeof(uint16_t)
                     + sizeof(uint16_t)};

    if (buffer.size() < size) {
        throw std::length_error{"Not enough buffer size"};
    }

    const uint8_t* ptr{buffer.data()};

    uint16_t message_size;
    std::memcpy(&message_size, ptr, sizeof(message_size));
    ptr += sizeof(message_size);

    size += message_size;

    signed_message.message = {ptr, ptr + message_size};
    ptr += message_size;

    uint16_t signature_size;
    std::memcpy(&signature_size, ptr, sizeof(signature_size));
    ptr += sizeof(signature_size);

    size += signature_size;

    signed_message.signature = {ptr, ptr + signature_size};
    ptr += signature_size;

    if (buffer.size() < size) {
        throw std::length_error{"Not enough buffer size"};
    }

    return signed_message;
}

bool MessageDeserializer::check_signature(const SignedMessage& signed_message, std::shared_ptr<dht::crypto::PublicKey> public_key) {
    return public_key->checkSignature(signed_message.message, signed_message.signature);
}

Message MessageDeserializer::message_from_buffer(const std::vector<uint8_t>& buffer, std::shared_ptr<dht::crypto::PrivateKey> private_key) {
    Message message{};

    size_t size{sizeof(message.id)
                     + sizeof(uint8_t) // source_ip_size
                     + sizeof(uint16_t) // source_port_size
                     + sizeof(uint8_t) // source_login_size
                     + sizeof(uint16_t) // source_public_key_size
                     + sizeof(uint8_t) // destination_ip_size
                     + sizeof(uint16_t) // destination_port_size
                     + sizeof(uint8_t) // destination_login_size
                     + sizeof(uint16_t) // destination_public_key_size
                     + sizeof(uint16_t)}; // payload_buffer_size

    if (buffer.size() < size) {
        throw std::length_error{"Not enough buffer size"};
    }

    const uint8_t* ptr{buffer.data()};

    std::memcpy(&message.id, ptr, sizeof(message.id));
    ptr += sizeof(message.id);

    uint8_t source_ip_size;
    std::memcpy(&source_ip_size, ptr, sizeof(source_ip_size));
    ptr += sizeof(source_ip_size);

    size += source_ip_size;

    message.source_ip = {ptr, ptr + source_ip_size};
    ptr += source_ip_size;

    std::memcpy(&message.source_port, ptr, sizeof(message.source_port));
    ptr += sizeof(message.source_port);

    uint8_t source_login_size;
    std::memcpy(&source_login_size, ptr, sizeof(source_login_size));
    ptr += sizeof(source_login_size);

    size += source_login_size;

    message.source_login = {ptr, ptr + source_login_size};
    ptr += source_login_size;

    uint16_t source_public_key_size;
    std::memcpy(&source_public_key_size, ptr, sizeof(source_public_key_size));
    ptr += sizeof(source_public_key_size);

    size += source_public_key_size;

    message.source_public_key = {ptr, ptr + source_public_key_size};
    ptr += source_public_key_size;

    uint8_t destination_ip_size;
    std::memcpy(&destination_ip_size, ptr, sizeof(destination_ip_size));
    ptr += sizeof(destination_ip_size);

    size += destination_ip_size;

    message.destination_ip = {ptr, ptr + destination_ip_size};
    ptr += destination_ip_size;

    std::memcpy(&message.destination_port, ptr, sizeof(message.destination_port));
    ptr += sizeof(message.destination_port);

    uint8_t destination_login_size;
    std::memcpy(&destination_login_size, ptr, sizeof(destination_login_size));
    ptr += sizeof(destination_login_size);

    size += destination_login_size;

    message.destination_login = {ptr, ptr + destination_login_size};
    ptr += destination_login_size;

    uint16_t destination_public_key_size;
    std::memcpy(&destination_public_key_size, ptr, sizeof(destination_public_key_size));
    ptr += sizeof(destination_public_key_size);

    size += destination_public_key_size;

    message.destination_public_key = {ptr, ptr + destination_public_key_size};
    ptr += destination_public_key_size;

    uint16_t payload_buffer_size;
    std::memcpy(&payload_buffer_size, ptr, sizeof(payload_buffer_size));
    ptr += sizeof(payload_buffer_size);

    size += payload_buffer_size;

    if (buffer.size() < size) {
        throw std::length_error{"Not enough buffer size"};
    }

    std::vector<uint8_t> payload_buffer{ptr, ptr + payload_buffer_size};
    ptr += payload_buffer_size;

    payload_buffer = PayloadDeserializer::decrypt(payload_buffer, *private_key);

    message.payload = PayloadDeserializer::payload_from_buffer(payload_buffer);

    return message;
}

Message MessageDeserializer::message_with_not_decrypted_payload_from_buffer(const std::vector<uint8_t>& buffer) {
    Message message{};

    size_t size{sizeof(message.id)
                     + sizeof(uint8_t) // source_ip_size
                     + sizeof(uint16_t) // source_port_size
                     + sizeof(uint8_t) // source_login_size
                     + sizeof(uint16_t) // source_public_key_size
                     + sizeof(uint8_t) // destination_ip_size
                     + sizeof(uint16_t) // destination_port_size
                     + sizeof(uint8_t) // destination_login_size
                     + sizeof(uint16_t) // destination_public_key_size
                     + sizeof(uint16_t)}; // payload_buffer_size

    if (buffer.size() < size) {
        throw std::length_error{"Not enough buffer size"};
    }

    const uint8_t* ptr{buffer.data()};

    std::memcpy(&message.id, ptr, sizeof(message.id));
    ptr += sizeof(message.id);

    uint8_t source_ip_size;
    std::memcpy(&source_ip_size, ptr, sizeof(source_ip_size));
    ptr += sizeof(source_ip_size);

    size += source_ip_size;

    message.source_ip = {ptr, ptr + source_ip_size};
    ptr += source_ip_size;

    std::memcpy(&message.source_port, ptr, sizeof(message.source_port));
    ptr += sizeof(message.source_port);

    uint8_t source_login_size;
    std::memcpy(&source_login_size, ptr, sizeof(source_login_size));
    ptr += sizeof(source_login_size);

    size += source_login_size;

    message.source_login = {ptr, ptr + source_login_size};
    ptr += source_login_size;

    uint16_t source_public_key_size;
    std::memcpy(&source_public_key_size, ptr, sizeof(source_public_key_size));
    ptr += sizeof(source_public_key_size);

    size += source_public_key_size;

    message.source_public_key = {ptr, ptr + source_public_key_size};
    ptr += source_public_key_size;

    uint8_t destination_ip_size;
    std::memcpy(&destination_ip_size, ptr, sizeof(destination_ip_size));
    ptr += sizeof(destination_ip_size);

    size += destination_ip_size;

    message.destination_ip = {ptr, ptr + destination_ip_size};
    ptr += destination_ip_size;

    std::memcpy(&message.destination_port, ptr, sizeof(message.destination_port));
    ptr += sizeof(message.destination_port);

    uint8_t destination_login_size;
    std::memcpy(&destination_login_size, ptr, sizeof(destination_login_size));
    ptr += sizeof(destination_login_size);

    size += destination_login_size;

    message.destination_login = {ptr, ptr + destination_login_size};
    ptr += destination_login_size;

    uint16_t destination_public_key_size;
    std::memcpy(&destination_public_key_size, ptr, sizeof(destination_public_key_size));
    ptr += sizeof(destination_public_key_size);

    size += destination_public_key_size;

    message.destination_public_key = {ptr, ptr + destination_public_key_size};
    ptr += destination_public_key_size;

    uint16_t payload_buffer_size;
    std::memcpy(&payload_buffer_size, ptr, sizeof(payload_buffer_size));
    ptr += sizeof(payload_buffer_size);

    size += payload_buffer_size;

    if (buffer.size() < size) {
        throw std::length_error{"Not enough buffer size"};
    }

    return message;
}

std::string MessageDeserializer::destinantion_public_key_from_buffer(const std::vector<uint8_t>& buffer) {
    const uint8_t* ptr{buffer.data()};

    ptr += sizeof(Message::id);

    uint8_t source_ip_size;
    std::memcpy(&source_ip_size, ptr, sizeof(source_ip_size));
    ptr += sizeof(source_ip_size);

    ptr += source_ip_size;

    ptr += sizeof(Message::source_port);

    uint8_t source_login_size;
    std::memcpy(&source_login_size, ptr, sizeof(source_login_size));
    ptr += sizeof(source_login_size);

    ptr += source_login_size;

    uint16_t source_public_key_size;
    std::memcpy(&source_public_key_size, ptr, sizeof(source_public_key_size));
    ptr += sizeof(source_public_key_size);

    ptr += source_public_key_size;

    uint8_t destination_ip_size;
    std::memcpy(&destination_ip_size, ptr, sizeof(destination_ip_size));
    ptr += sizeof(destination_ip_size);

    ptr += destination_ip_size;

    ptr += sizeof(Message::destination_port);

    uint8_t destination_login_size;
    std::memcpy(&destination_login_size, ptr, sizeof(destination_login_size));
    ptr += sizeof(destination_login_size);

    ptr += destination_login_size;

    uint16_t destination_public_key_size;
    std::memcpy(&destination_public_key_size, ptr, sizeof(destination_public_key_size));
    ptr += sizeof(destination_public_key_size);

    return {ptr, ptr + destination_public_key_size};
}