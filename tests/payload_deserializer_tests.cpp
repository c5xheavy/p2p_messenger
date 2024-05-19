#include <catch2/catch.hpp>

#include "../src/payload_deserializer.h"

TEST_CASE("Payload Deserialization") {
    SECTION("integer time") {
        json::object json_payload {
            { "time", 1700000000 },
            { "text", "hello"}
        };
        Payload payload{PayloadDeserializer::payload_from_json(json_payload)};
        REQUIRE(payload.time == 1700000000);
        REQUIRE(payload.text == "hello");
    }

    // time is always integer
    /*
    SECTION("floating point time") {
        json::object json_payload {
            { "time", 1700000000.5 },
            { "text", "hello"}
        };
        Payload payload{PayloadDeserializer::payload_from_json(json_payload)};
        REQUIRE(payload.time == 1700000000);
        REQUIRE(payload.text == "hello");
    }
    */

    SECTION("empty string in text field") {
        json::object json_payload {
            { "time", 0 },
            { "text", ""}
        };
        Payload payload{PayloadDeserializer::payload_from_json(json_payload)};
        REQUIRE(payload.time == 0);
        REQUIRE(payload.text == "");
    }

    SECTION("unnecessary extra fields") {
        json::object json_payload {
            { "some unnecessary extra field 1", 5},
            { "time", 1700000000 },
            { "text", "hello"},
            { "some unnecessary extra field 2", true }
        };
        Payload payload{PayloadDeserializer::payload_from_json(json_payload)};
        REQUIRE(payload.time == 1700000000);
        REQUIRE(payload.text == "hello");
    }

    SECTION("no required time field") {
        json::object json_payload {
            { "some unnecessary extra field 1", 5},
            { "text", "hello"},
            { "some unnecessary extra field 2", true }
        };
        REQUIRE_THROWS(PayloadDeserializer::payload_from_json(json_payload));
    }

    SECTION("no fields") {
        json::object json_payload{};
        REQUIRE_THROWS(PayloadDeserializer::payload_from_json(json_payload));
    }

    SECTION("string time") {
        json::object json_payload {
            { "time", "1700000000" },
            { "text", "hello"}
        };
        REQUIRE_THROWS(PayloadDeserializer::payload_from_json(json_payload));
    }

    SECTION("boolean text") {
        json::object json_payload {
            { "time", 1700000000 },
            { "text", false}
        };
        REQUIRE_THROWS(PayloadDeserializer::payload_from_json(json_payload));
    }
}
