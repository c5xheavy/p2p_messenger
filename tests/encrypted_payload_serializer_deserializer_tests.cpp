#include <catch2/catch.hpp>
#include <opendht.h>

#include "../src/payload.h"
#include "../src/payload_serializer.h"
#include "../src/payload_deserializer.h"
#include "payload_utils.h"

TEST_CASE("Encrypted Payload Serialization & Deserialization") {
    SECTION("encrypting and decrypting with the same RSA key pair") {
        Payload payload {
            1700000000,
            "hello"
        };
        dht::crypto::Identity identity{dht::crypto::generateIdentity()};
        std::vector<uint8_t> buffer{PayloadSerializer::payload_to_buffer(payload)};
        std::vector<uint8_t> encrypted_buffer{PayloadSerializer::encrypt(buffer, identity.first->getPublicKey())};
        std::vector<uint8_t> decrypted_buffer{PayloadDeserializer::decrypt(encrypted_buffer, *identity.first)};
        Payload post_payload{PayloadDeserializer::payload_from_buffer(decrypted_buffer)};
        REQUIRE(payload == post_payload);
    }

    SECTION("encrypting with one RSA key pair and decrypting with another") {
        Payload payload {
            1700000000,
            "hello"
        };
        dht::crypto::Identity identity1{dht::crypto::generateIdentity()};
        std::vector<uint8_t> buffer{PayloadSerializer::payload_to_buffer(payload)};
        std::vector<uint8_t> encrypted_buffer{PayloadSerializer::encrypt(buffer, identity1.first->getPublicKey())};
        dht::crypto::Identity identity2{dht::crypto::generateIdentity()};
        REQUIRE_THROWS(PayloadDeserializer::decrypt(encrypted_buffer, *identity2.first));
    }
}
