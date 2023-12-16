#include "payload_serializer.h"

json::object PayloadSerializer::PayloadToJson(const Payload& payload) { 
    return {
        { "time", payload.time },
        { "text", payload.text }
    };
}
