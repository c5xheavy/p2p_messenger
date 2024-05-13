#include "payload_serializer.h"

#include <boost/json.hpp>

#include "payload.h"

namespace json = boost::json;

json::object PayloadSerializer::payload_to_json(const Payload& payload) { 
    return {
        { "time", payload.time },
        { "text", payload.text }
    };
}
