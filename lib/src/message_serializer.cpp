#include "message_serializer.h"

json::object MessageSerializer::MessageToJson(const Message& message) { 
    return {
        { "senderIp", message.sender_ip },
        { "time", message.time },
        { "text", message.text }
    };
}
