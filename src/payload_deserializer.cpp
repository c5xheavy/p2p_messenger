#include "payload_deserializer.h"

#include <cstdint>
#include <string>

#include <boost/json.hpp>

#include "payload.h"

namespace json = boost::json;

Payload PayloadDeserializer::payload_from_buffer(const std::vector<uint8_t>& buffer) {
    std::string str_payload{buffer.begin(), buffer.end()};
    json::value json_payload = json::parse(str_payload).as_object();
    return {
        json_payload.at("time").to_number<std::uint64_t>(),
        json::value_to<std::string>(json_payload.at("text"))
    };
}
