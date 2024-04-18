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

Message MessageDeserializer::MessageFromBuffer(char* buffer, std::size_t buffer_size) {
    Message message{};

    std::size_t size{sizeof(message.id)
                     + 2 * Message::login_size
                     + sizeof(std::uint16_t)};

    if (buffer_size < size) {
        throw std::length_error{"Not enough buffer size"};
    }

    std::memcpy(&message.id, buffer, sizeof(message.id));
    buffer += sizeof(message.id);

    message.source_login = {buffer, Message::login_size};
    buffer += Message::login_size;

    message.destination_login = {buffer, Message::login_size};
    buffer += Message::login_size;

    std::uint16_t payload_size;
    std::memcpy(&payload_size, buffer, sizeof(payload_size));
    buffer += sizeof(payload_size);

    size += payload_size;

    if (buffer_size < size) {
        throw std::length_error{"Not enough buffer size"};
    }

    std::string str_payload{buffer, payload_size};
    buffer += payload_size;

    message.payload = PayloadDeserializer::PayloadFromJson(json::parse(str_payload).as_object());
    return message;
}
