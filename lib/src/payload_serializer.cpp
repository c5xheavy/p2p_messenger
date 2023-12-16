#include "payload_serializer.h"

json::object PayloadSerializer::PayloadToJson(const Payload& payload) { 
    return {
        { "senderIp", payload.sender_ip },
        { "time", payload.time },
        { "text", payload.text }
    };
}
