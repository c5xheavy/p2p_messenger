#ifndef PAYLOAD_DESERIALIZER_H
#define PAYLOAD_DESERIALIZER_H

#include <boost/json.hpp>

#include "payload.h"

namespace json = boost::json;

class PayloadDeserializer {
public:
    static Payload payload_from_json(const json::object& json_payload);
};

#endif // PAYLOAD_DESERIALIZER_H
