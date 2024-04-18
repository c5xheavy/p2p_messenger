#include "message_serializer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <utility>

#include <boost/json.hpp>

#include "message.h"
#include "payload_serializer.h"

namespace json = boost::json;

std::pair<std::shared_ptr<char[]>, std::size_t> MessageSerializer::MessageToBuffer(const Message& message) {
    std::string str_payload{json::serialize(PayloadSerializer::PayloadToJson(message.payload))};

    std::size_t size{sizeof(message.id)
                     + sizeof(std::uint8_t) + message.source_login.size()
                     + sizeof(std::uint8_t) + message.destination_login.size()
                     + sizeof(std::uint16_t) + str_payload.size()};

    const std::shared_ptr<char[]> buffer{new char[size]};
    char* ptr{buffer.get()};

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

    std::uint16_t str_payload_size{static_cast<std::uint16_t>(str_payload.size())};
    std::memcpy(ptr, &str_payload_size, sizeof(str_payload_size));
    ptr += sizeof(str_payload_size);

    std::memcpy(ptr, str_payload.data(), str_payload_size);
    ptr += str_payload_size;

    return {buffer, size};
}
