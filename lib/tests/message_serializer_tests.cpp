#include <catch2/catch.hpp>

#include "../src/message_serializer.h"

using Catch::Matchers::WithinAbs;

TEST_CASE("Message Serialization") {
    SECTION("integer time") {
        Message message{
            "127.0.0.1",
            1700000000,
            "hello"
        };
        json::object jsonMessage{MessageSerializer::MessageToJson(message)};
        REQUIRE(jsonMessage.at("senderIp") == "127.0.0.1");
        REQUIRE(jsonMessage.at("time").as_int64() == 1700000000);
        REQUIRE(jsonMessage.at("text") == "hello");
    }

    // narrowing conversion error with -Wnarrowing
    // idk should i supress it

    /*
    SECTION("floating point time") {
        Message message{
            "127.0.0.1",
            1700000000.5,
            "hello"
        };
        json::object jsonMessage{MessageSerializer::MessageToJson(message)};
        REQUIRE(jsonMessage.at("senderIp") == "127.0.0.1");
        REQUIRE_THAT(jsonMessage.at("time").as_int64() == 1700000000);
        REQUIRE(jsonMessage.at("text") == "hello");
    }
    */

    SECTION("empty strings in senderIp and text fields") {
        Message message{
            "",
            0,
            ""
        };
        json::object jsonMessage{MessageSerializer::MessageToJson(message)};
        REQUIRE(jsonMessage.at("senderIp") == "");
        REQUIRE(jsonMessage.at("time").as_int64() == 0);
        REQUIRE(jsonMessage.at("text") == "");
    }

    SECTION("no required text field") {
        Message message{
            "127.0.0.1",
            1700000000
        };
        json::object jsonMessage{MessageSerializer::MessageToJson(message)};
        REQUIRE(jsonMessage.at("senderIp") == "127.0.0.1");
        REQUIRE(jsonMessage.at("time").as_int64() == 1700000000);
        REQUIRE(jsonMessage.at("text") == "");
    }

    SECTION("no fields") {
        Message message{};
        json::object jsonMessage{MessageSerializer::MessageToJson(message)};
        REQUIRE(jsonMessage.at("senderIp") == "");
        REQUIRE(jsonMessage.at("time").as_int64() == 0);
        REQUIRE(jsonMessage.at("text") == "");
    }
}
