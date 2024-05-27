#include "message_deserializer.h"

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/json.hpp>

#include "message.h"
#include "payload_deserializer.h"

namespace json = boost::json;

Message MessageDeserializer::message_from_buffer(const std::vector<uint8_t>& buffer) {
    Message message{};

    std::size_t size{sizeof(message.id)
                     + sizeof(std::uint8_t)
                     + sizeof(std::uint8_t)
                     + sizeof(std::uint16_t)};

    if (buffer.size() < size) {
        throw std::length_error{"Not enough buffer size"};
    }

    const uint8_t* ptr{buffer.data()};

    std::memcpy(&message.id, ptr, sizeof(message.id));
    ptr += sizeof(message.id);

    std::uint8_t source_login_size;
    std::memcpy(&source_login_size, ptr, sizeof(source_login_size));
    ptr += sizeof(source_login_size);

    size += source_login_size;

    message.source_login = {ptr, ptr + source_login_size};
    ptr += source_login_size;

    std::uint8_t destination_login_size;
    std::memcpy(&destination_login_size, ptr, sizeof(destination_login_size));
    ptr += sizeof(destination_login_size);

    size += destination_login_size;

    message.destination_login = {ptr, ptr + destination_login_size};
    ptr += destination_login_size;

    std::uint16_t payload_buffer_size;
    std::memcpy(&payload_buffer_size, ptr, sizeof(payload_buffer_size));
    ptr += sizeof(payload_buffer_size);

    size += payload_buffer_size;

    if (buffer.size() < size) {
        throw std::length_error{"Not enough buffer size"};
    }

    std::vector<uint8_t> payload_buffer{ptr, ptr + payload_buffer_size};
    ptr += payload_buffer_size;

    message.payload = PayloadDeserializer::payload_from_buffer(payload_buffer);
    return message;
}
