#include "payload_serializer.h"

#include <boost/json.hpp>

#include "payload.h"

namespace json = boost::json;

json::object PayloadSerializer::PayloadToJson(const Payload& payload) { 
    return {
        { "time", payload.time },
        { "text", payload.text }
    };
}
