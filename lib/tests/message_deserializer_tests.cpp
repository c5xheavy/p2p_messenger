#include <catch2/catch.hpp>

#include "../src/message_deserializer.h"

TEST_CASE("Message Deserialization") {
    SECTION("integer time") {
        json::object jsonMessage{
            { "senderIp", "127.0.0.1" },
            { "time", 1700000000 },
            { "text", "hello"}
        };
        Message message{MessageDeserializer::MessageFromJson(jsonMessage)};
        REQUIRE(message.sender_ip == "127.0.0.1");
        REQUIRE(message.time == 1700000000);
        REQUIRE(message.text == "hello");
    }

    SECTION("floating point time") {
        json::object jsonMessage{
            { "senderIp", "127.0.0.1" },
            { "time", 1700000000.5 },
            { "text", "hello"}
        };
        Message message{MessageDeserializer::MessageFromJson(jsonMessage)};
        REQUIRE(message.sender_ip == "127.0.0.1");
        REQUIRE(message.time == 1700000000);
        REQUIRE(message.text == "hello");
    }

    SECTION("empty strings in senderIp and text fields") {
        json::object jsonMessage{
            { "senderIp", "" },
            { "time", 0 },
            { "text", ""}
        };
        Message message{MessageDeserializer::MessageFromJson(jsonMessage)};
        REQUIRE(message.sender_ip == "");
        REQUIRE(message.time == 0);
        REQUIRE(message.text == "");
    }

    SECTION("unnecessary extra fields") {
        json::object jsonMessage{
            { "senderIp", "127.0.0.1" },
            { "some unnecessary extra field 1", 5},
            { "time", 1700000000 },
            { "text", "hello"},
            { "some unnecessary extra field 2", true }
        };
        Message message{MessageDeserializer::MessageFromJson(jsonMessage)};
        REQUIRE(message.sender_ip == "127.0.0.1");
        REQUIRE(message.time == 1700000000);
        REQUIRE(message.text == "hello");
    }

    SECTION("no required time field") {
        json::object jsonMessage{
            { "senderIp", "127.0.0.1" },
            { "some unnecessary extra field 1", 5},
            { "text", "hello"},
            { "some unnecessary extra field 2", true }
        };
        REQUIRE_THROWS(MessageDeserializer::MessageFromJson(jsonMessage));
    }

    SECTION("no fields") {
        json::object jsonMessage{};
        REQUIRE_THROWS(MessageDeserializer::MessageFromJson(jsonMessage));
    }

    SECTION("string time") {
        json::object jsonMessage{
            { "senderIp", "127.0.0.1" },
            { "time", "1700000000" },
            { "text", "hello"}
        };
        REQUIRE_THROWS(MessageDeserializer::MessageFromJson(jsonMessage));
    }

    SECTION("integer senderIp") {
        json::object jsonMessage{
            { "senderIp", 127000000001 },
            { "time", 1700000000 },
            { "text", "hello"}
        };
        REQUIRE_THROWS(MessageDeserializer::MessageFromJson(jsonMessage));
    }

    SECTION("boolean text") {
        json::object jsonMessage{
            { "senderIp", "127.0.0.1" },
            { "time", 1700000000 },
            { "text", false}
        };
        REQUIRE_THROWS(MessageDeserializer::MessageFromJson(jsonMessage));
    }
}
