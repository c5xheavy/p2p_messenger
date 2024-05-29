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
            "user1",
            "public_key1",
            "user2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message)};
        Message post_message{MessageDeserializer::message_from_buffer(buffer)};
        REQUIRE(message == post_message);
    }

    SECTION("64 byte sized logins") {
        Message message {
            1,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "public_key1",
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message)};
        Message post_message{MessageDeserializer::message_from_buffer(buffer)};
        REQUIRE(message == post_message);
    }

    SECTION("big id") {
        Message message {
            999999999999999,
            "user1",
            "public_key1",
            "user2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message)};
        Message post_message{MessageDeserializer::message_from_buffer(buffer)};
        REQUIRE(message == post_message);
    }

    SECTION("big everything") {
        Payload long_payload {
            1700000000,
            "hellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohello"
        };
        Message message {
            123123123123123,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "public_key1",
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
            long_payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message)};
        Message post_message{MessageDeserializer::message_from_buffer(buffer)};
        REQUIRE(message == post_message);
    }

    SECTION("special symbols logins") {
        Message message {
            1,
            "qweasdzxcrtyfghvbnuiojklm,.p[];'/1234567689   53742y52734805",
            "public_key1",
            "раводлыофврадцукенщцшгукрповмтоывмтьлждьфылоывшщ15468768   ",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message)};
        Message post_message{MessageDeserializer::message_from_buffer(buffer)};
        REQUIRE(message == post_message);
    }
}
