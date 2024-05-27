#include "payload_serializer.h"

#include <boost/json.hpp>

#include "payload.h"

namespace json = boost::json;

std::vector<uint8_t> PayloadSerializer::payload_to_buffer(const Payload& payload) {
    json::object json_payload {
        { "time", payload.time },
        { "text", payload.text }
    };
    std::string str_payload = json::serialize(json_payload);
    std::vector<uint8_t> buffer{str_payload.begin(), str_payload.end()};
    return buffer;
}
