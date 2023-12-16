#include <catch2/catch.hpp>

#include "../src/payload_deserializer.h"

TEST_CASE("Payload Deserialization") {
    SECTION("integer time") {
        json::object jsonPayload {
            { "senderIp", "127.0.0.1" },
            { "time", 1700000000 },
            { "text", "hello"}
        };
        Payload payload{PayloadDeserializer::PayloadFromJson(jsonPayload)};
        REQUIRE(payload.sender_ip == "127.0.0.1");
        REQUIRE(payload.time == 1700000000);
        REQUIRE(payload.text == "hello");
    }

    // time is always integer
    /*
    SECTION("floating point time") {
        json::object jsonPayload {
            { "senderIp", "127.0.0.1" },
            { "time", 1700000000.5 },
            { "text", "hello"}
        };
        Payload payload{PayloadDeserializer::PayloadFromJson(jsonPayload)};
        REQUIRE(payload.sender_ip == "127.0.0.1");
        REQUIRE(payload.time == 1700000000);
        REQUIRE(payload.text == "hello");
    }
    */

    SECTION("empty strings in senderIp and text fields") {
        json::object jsonPayload {
            { "senderIp", "" },
            { "time", 0 },
            { "text", ""}
        };
        Payload payload{PayloadDeserializer::PayloadFromJson(jsonPayload)};
        REQUIRE(payload.sender_ip == "");
        REQUIRE(payload.time == 0);
        REQUIRE(payload.text == "");
    }

    SECTION("unnecessary extra fields") {
        json::object jsonPayload {
            { "senderIp", "127.0.0.1" },
            { "some unnecessary extra field 1", 5},
            { "time", 1700000000 },
            { "text", "hello"},
            { "some unnecessary extra field 2", true }
        };
        Payload payload{PayloadDeserializer::PayloadFromJson(jsonPayload)};
        REQUIRE(payload.sender_ip == "127.0.0.1");
        REQUIRE(payload.time == 1700000000);
        REQUIRE(payload.text == "hello");
    }

    SECTION("no required time field") {
        json::object jsonPayload {
            { "senderIp", "127.0.0.1" },
            { "some unnecessary extra field 1", 5},
            { "text", "hello"},
            { "some unnecessary extra field 2", true }
        };
        REQUIRE_THROWS(PayloadDeserializer::PayloadFromJson(jsonPayload));
    }

    SECTION("no fields") {
        json::object jsonPayload{};
        REQUIRE_THROWS(PayloadDeserializer::PayloadFromJson(jsonPayload));
    }

    SECTION("string time") {
        json::object jsonPayload {
            { "senderIp", "127.0.0.1" },
            { "time", "1700000000" },
            { "text", "hello"}
        };
        REQUIRE_THROWS(PayloadDeserializer::PayloadFromJson(jsonPayload));
    }

    SECTION("integer senderIp") {
        json::object jsonPayload {
            { "senderIp", 127000000001 },
            { "time", 1700000000 },
            { "text", "hello"}
        };
        REQUIRE_THROWS(PayloadDeserializer::PayloadFromJson(jsonPayload));
    }

    SECTION("boolean text") {
        json::object jsonPayload {
            { "senderIp", "127.0.0.1" },
            { "time", 1700000000 },
            { "text", false}
        };
        REQUIRE_THROWS(PayloadDeserializer::PayloadFromJson(jsonPayload));
    }
}
