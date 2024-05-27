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

    std::uint16_t str_payload_size;
    std::memcpy(&str_payload_size, ptr, sizeof(str_payload_size));
    ptr += sizeof(str_payload_size);

    size += str_payload_size;

    if (buffer.size() < size) {
        throw std::length_error{"Not enough buffer size"};
    }

    std::string str_payload{ptr, ptr + str_payload_size};
    ptr += str_payload_size;

    message.payload = PayloadDeserializer::payload_from_json(json::parse(str_payload).as_object());
    return message;
}
