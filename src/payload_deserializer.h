#ifndef PAYLOAD_DESERIALIZER_H
#define PAYLOAD_DESERIALIZER_H

#include <cstdint>
#include <vector>

#include <boost/json.hpp>
#include <opendht.h>

#include "payload.h"

namespace json = boost::json;

class PayloadDeserializer {
public:
    static Payload payload_from_buffer(const std::vector<uint8_t>& buffer);
    static std::vector<uint8_t> decrypt(const std::vector<uint8_t>& buffer, const dht::crypto::PrivateKey& private_key);
};

#endif // PAYLOAD_DESERIALIZER_H
