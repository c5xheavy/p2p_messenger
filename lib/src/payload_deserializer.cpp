#include "payload_deserializer.h"

Payload PayloadDeserializer::PayloadFromJson(const json::object& jsonPayload) {
    return {
        json::value_to<std::string>(jsonPayload.at("senderIp")),
        jsonPayload.at("time").as_int64(),
        json::value_to<std::string>(jsonPayload.at("text"))
    };
}
