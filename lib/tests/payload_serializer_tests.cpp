#include <catch2/catch.hpp>

#include "../src/payload_serializer.h"

TEST_CASE("Payload Serialization") {
    SECTION("integer time") {
        Payload payload {
            1700000000,
            "hello"
        };
        json::object jsonPayload{PayloadSerializer::payload_to_json(payload)};
        REQUIRE(jsonPayload.at("time").to_number<std::uint64_t>() == 1700000000);
        REQUIRE(jsonPayload.at("text") == "hello");
    }

    // narrowing conversion error with -Wnarrowing
    // idk should i supress it

    /*
    SECTION("floating point time") {
        Payload payload {
            1700000000.5,
            "hello"
        };
        json::object jsonPayload{PayloadSerializer::payload_to_json(payload)};
        REQUIRE_THAT(jsonPayload.at("time").to_number<std::uint64_t>() == 1700000000);
        REQUIRE(jsonPayload.at("text") == "hello");
    }
    */

    SECTION("empty string in text field") {
        Payload payload {
            0,
            ""
        };
        json::object jsonPayload{PayloadSerializer::payload_to_json(payload)};
        REQUIRE(jsonPayload.at("time").to_number<std::uint64_t>() == 0);
        REQUIRE(jsonPayload.at("text") == "");
    }

    SECTION("no required text field") {
        Payload payload {
            1700000000,
            {}
        };
        json::object jsonPayload{PayloadSerializer::payload_to_json(payload)};
        REQUIRE(jsonPayload.at("time").to_number<std::uint64_t>() == 1700000000);
        REQUIRE(jsonPayload.at("text") == "");
    }

    SECTION("no fields") {
        Payload payload{};
        json::object jsonPayload{PayloadSerializer::payload_to_json(payload)};
        REQUIRE(jsonPayload.at("time").to_number<std::uint64_t>() == 0);
        REQUIRE(jsonPayload.at("text") == "");
    }
}
