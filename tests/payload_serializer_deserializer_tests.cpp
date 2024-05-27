#include <catch2/catch.hpp>

#include "../src/payload_serializer.h"
#include "../src/payload_deserializer.h"
#include "payload_utils.h"

TEST_CASE("Payload Serialization & Deserialization") {
    SECTION("integer time") {
        Payload payload {
            1700000000,
            "hello"
        };
        std::vector<uint8_t> buffer{PayloadSerializer::payload_to_buffer(payload)};
        Payload post_payload{PayloadDeserializer::payload_from_buffer(buffer)};
        REQUIRE(payload == post_payload);
    }

    SECTION("empty string in text field") {
        Payload payload {
            0,
            ""
        };
        std::vector<uint8_t> buffer{PayloadSerializer::payload_to_buffer(payload)};
        Payload post_payload{PayloadDeserializer::payload_from_buffer(buffer)};
        REQUIRE(payload == post_payload);
    }

    SECTION("no required text field") {
        Payload payload {
            1700000000,
            {}
        };
        std::vector<uint8_t> buffer{PayloadSerializer::payload_to_buffer(payload)};
        Payload post_payload{PayloadDeserializer::payload_from_buffer(buffer)};
        REQUIRE(payload == post_payload);
    }

    SECTION("no fields") {
        Payload payload{};
        std::vector<uint8_t> buffer{PayloadSerializer::payload_to_buffer(payload)};
        Payload post_payload{PayloadDeserializer::payload_from_buffer(buffer)};
        REQUIRE(payload == post_payload);
    }
}
