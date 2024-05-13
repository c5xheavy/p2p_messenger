#include "payload_deserializer.h"

#include <cstdint>
#include <string>

#include <boost/json.hpp>

#include "payload.h"

namespace json = boost::json;

Payload PayloadDeserializer::payload_from_json(const json::object& json_payload) {
    return {
        json_payload.at("time").to_number<std::uint64_t>(),
        json::value_to<std::string>(json_payload.at("text"))
    };
}
