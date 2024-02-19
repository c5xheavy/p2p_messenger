#include "payload_deserializer.h"

#include <cstdint>
#include <string>

#include <boost/json.hpp>

#include "payload.h"

namespace json = boost::json;

Payload PayloadDeserializer::PayloadFromJson(const json::object& jsonPayload) {
    return {
        jsonPayload.at("time").to_number<std::uint64_t>(),
        json::value_to<std::string>(jsonPayload.at("text"))
    };
}
