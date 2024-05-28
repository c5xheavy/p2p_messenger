#ifndef PAYLOAD_SERIALIZER_H
#define PAYLOAD_SERIALIZER_H

#include <cstdint>
#include <vector>

#include <boost/json.hpp>
#include <opendht.h>

#include "payload.h"

namespace json = boost::json;

class PayloadSerializer {
public:
    static std::vector<uint8_t> payload_to_buffer(const Payload& payload);
    static std::vector<uint8_t> encrypt(const std::vector<uint8_t>& buffer, const dht::crypto::PublicKey& public_key);
};

#endif // PAYLOAD_SERIALIZER_H
