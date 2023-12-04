#include "message_deserializer.h"

Message MessageDeserializer::MessageFromJson(const json::object& jsonMessage) {
    if (jsonMessage.at("time").if_double()) {
        return {
            json::value_to<std::string>(jsonMessage.at("senderIp")),
            static_cast<int64_t>(jsonMessage.at("time").as_double()),
            json::value_to<std::string>(jsonMessage.at("text"))
        };
    }
    return {
        json::value_to<std::string>(jsonMessage.at("senderIp")),
        jsonMessage.at("time").as_int64(),
        json::value_to<std::string>(jsonMessage.at("text"))
    };
}
