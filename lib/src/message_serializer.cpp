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
                     + 2 * Message::login_size
                     + sizeof(std::uint16_t) + str_payload.size()};

    const std::shared_ptr<char[]> buffer{new char[size]};
    char* ptr{buffer.get()};

    std::memcpy(ptr, &message.id, sizeof(message.id));
    ptr += sizeof(message.id);

    std::memcpy(ptr, message.source_login.data(), message.source_login.size());
    ptr += message.source_login.size();

    std::memcpy(ptr, message.destination_login.data(), message.destination_login.size());
    ptr += message.destination_login.size();

    std::uint16_t str_payload_size{static_cast<std::uint16_t>(str_payload.size())};
    std::memcpy(ptr, &str_payload_size, sizeof(str_payload_size));
    ptr += sizeof(str_payload_size);

    std::memcpy(ptr, str_payload.data(), str_payload_size);
    ptr += str_payload_size;

    return {buffer, size};
}
