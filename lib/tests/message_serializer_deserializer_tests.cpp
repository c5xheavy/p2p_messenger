#include <catch2/catch.hpp>

#include "../src/message_serializer.h"
#include "../src/message_deserializer.h"
#include "message_utils.h"

TEST_CASE("Message Serialization & Deserialization") {

    Payload payload {
        1700000000,
        "hello"
    };

    SECTION("simple") {
        Message message {
            1,
            "user2",
            "user1",
            payload
        };
        Buffer buffer{MessageSerializer::MessageToBuffer(message)};
        Message post_message{MessageDeserializer::MessageFromBuffer(buffer)};
        REQUIRE(message == post_message);
    }

    SECTION("64 byte sized logins") {
        Message message {
            1,
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            payload
        };
        Buffer buffer{MessageSerializer::MessageToBuffer(message)};
        Message post_message{MessageDeserializer::MessageFromBuffer(buffer)};
        REQUIRE(message == post_message);
    }

    SECTION("big id") {
        Message message {
            999999999999999,
            "user2",
            "user1",
            payload
        };
        Buffer buffer{MessageSerializer::MessageToBuffer(message)};
        Message post_message{MessageDeserializer::MessageFromBuffer(buffer)};
        REQUIRE(message == post_message);
    }

    SECTION("big everything") {
        Payload long_payload {
            1700000000,
            "hellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohello"
        };
        Message message {
            123123123123123,
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            long_payload
        };
        Buffer buffer{MessageSerializer::MessageToBuffer(message)};
        Message post_message{MessageDeserializer::MessageFromBuffer(buffer)};
        REQUIRE(message == post_message);
    }

    SECTION("special symbols logins") {
        Message message {
            1,
            "раводлыофврадцукенщцшгукрповмтоывмтьлждьфылоывшщ15468768   ",
            "qweasdzxcrtyfghvbnuiojklm,.p[];'/1234567689   53742y52734805",
            payload
        };
        Buffer buffer{MessageSerializer::MessageToBuffer(message)};
        Message post_message{MessageDeserializer::MessageFromBuffer(buffer)};
        REQUIRE(message == post_message);
    }
}
