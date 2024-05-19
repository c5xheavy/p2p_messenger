#include "message_deserializer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>

#include <boost/json.hpp>

#include "message.h"
#include "payload_deserializer.h"

namespace json = boost::json;

Message MessageDeserializer::message_from_buffer(char* buffer, std::size_t buffer_size) {
    Message message{};

    std::size_t size{sizeof(message.id)
                     + sizeof(std::uint8_t)
                     + sizeof(std::uint8_t)
                     + sizeof(std::uint16_t)};

    if (buffer_size < size) {
        throw std::length_error{"Not enough buffer size"};
    }

    std::memcpy(&message.id, buffer, sizeof(message.id));
    buffer += sizeof(message.id);

    std::uint8_t source_login_size;
    std::memcpy(&source_login_size, buffer, sizeof(source_login_size));
    buffer += sizeof(source_login_size);

    size += source_login_size;

    message.source_login = {buffer, source_login_size};
    buffer += source_login_size;

    std::uint8_t destination_login_size;
    std::memcpy(&destination_login_size, buffer, sizeof(destination_login_size));
    buffer += sizeof(destination_login_size);

    size += destination_login_size;

    message.destination_login = {buffer, destination_login_size};
    buffer += destination_login_size;

    std::uint16_t str_payload_size;
    std::memcpy(&str_payload_size, buffer, sizeof(str_payload_size));
    buffer += sizeof(str_payload_size);

    size += str_payload_size;

    if (buffer_size < size) {
        throw std::length_error{"Not enough buffer size"};
    }

    std::string str_payload{buffer, str_payload_size};
    buffer += str_payload_size;

    message.payload = PayloadDeserializer::payload_from_json(json::parse(str_payload).as_object());
    return message;
}
