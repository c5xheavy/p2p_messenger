#ifndef PAYLOAD_DESERIALIZER_H
#define PAYLOAD_DESERIALIZER_H

#include <string>

#include <boost/json.hpp>

#include "payload.h"

namespace json = boost::json;

class PayloadDeserializer {
public:
    static Payload PayloadFromJson(const json::object& jsonPayload);
};

#endif
