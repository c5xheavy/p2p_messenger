#include "message_deserializer.h"

Message MessageDeserializer::MessageFromJson(const json::object& jsonMessage) {
    return {
        json::value_to<std::string>(jsonMessage.at("senderIp")),
        jsonMessage.at("time").as_int64(),
        json::value_to<std::string>(jsonMessage.at("text"))
    };
}
