#ifndef PAYLOAD_SERIALIZER_H
#define PAYLOAD_SERIALIZER_H

#include <boost/json.hpp>

#include "payload.h"

namespace json = boost::json;

class PayloadSerializer {
public:
    static std::vector<uint8_t> payload_to_buffer(const Payload& payload);
};

#endif // PAYLOAD_SERIALIZER_H
