#include "payload_deserializer.h"

Payload PayloadDeserializer::PayloadFromJson(const json::object& jsonPayload) {
    return {
        jsonPayload.at("time").as_int64(),
        json::value_to<std::string>(jsonPayload.at("text"))
    };
}
