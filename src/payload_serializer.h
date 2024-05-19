#ifndef PAYLOAD_SERIALIZER_H
#define PAYLOAD_SERIALIZER_H

#include <boost/json.hpp>

#include "payload.h"

namespace json = boost::json;

class PayloadSerializer {
public:
    static json::object payload_to_json(const Payload& payload);
};

#endif // PAYLOAD_SERIALIZER_H
