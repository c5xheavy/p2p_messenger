#include "payload_deserializer.h"

#include <cstdint>
#include <string>
#include <vector>

#include <boost/json.hpp>
#include <opendht.h>

#include "payload.h"

namespace json = boost::json;

Payload PayloadDeserializer::payload_from_buffer(const std::vector<uint8_t>& buffer) {
    std::string str_payload{buffer.begin(), buffer.end()};
    json::object json_payload = json::parse(str_payload).as_object();
    return {
        json_payload.at("time").to_number<std::uint64_t>(),
        json::value_to<std::string>(json_payload.at("text"))
    };
}

std::vector<uint8_t> PayloadDeserializer::decrypt(const std::vector<uint8_t>& buffer, const dht::crypto::PrivateKey& private_key) {
    return private_key.decrypt(buffer);
}
