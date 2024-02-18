#include <catch2/catch.hpp>

#include "../src/message_serializer.h"
#include "../src/message_deserializer.h"
#include "../src/login_hasher.h"
#include "message_utils.h"

TEST_CASE("Message Serialization & Deserialization") {

    Payload payload {
        1700000000,
        "hello"
    };

    SECTION("simple") {
        Message message {
            1,
            LoginHasher::Hash("user1"),
            LoginHasher::Hash("user2"),
            payload
        };
        auto [buffer, size] = MessageSerializer::MessageToBuffer(message);
        Message post_message{MessageDeserializer::MessageFromBuffer(buffer.get(), size)};
        REQUIRE(message == post_message);
    }

    SECTION("64 byte sized logins") {
        Message message {
            1,
            LoginHasher::Hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
            LoginHasher::Hash("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"),
            payload
        };
        auto [buffer, size] = MessageSerializer::MessageToBuffer(message);
        Message post_message{MessageDeserializer::MessageFromBuffer(buffer.get(), size)};
        REQUIRE(message == post_message);
    }

    SECTION("big id") {
        Message message {
            999999999999999,
            LoginHasher::Hash("user1"),
            LoginHasher::Hash("user2"),
            payload
        };
        auto [buffer, size] = MessageSerializer::MessageToBuffer(message);
        Message post_message{MessageDeserializer::MessageFromBuffer(buffer.get(), size)};
        REQUIRE(message == post_message);
    }

    SECTION("big everything") {
        Payload long_payload {
            1700000000,
            "hellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohello"
        };
        Message message {
            123123123123123,
            LoginHasher::Hash("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
            LoginHasher::Hash("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"),
            long_payload
        };
        auto [buffer, size] = MessageSerializer::MessageToBuffer(message);
        Message post_message{MessageDeserializer::MessageFromBuffer(buffer.get(), size)};
        REQUIRE(message == post_message);
    }

    SECTION("special symbols logins") {
        Message message {
            1,
            LoginHasher::Hash("qweasdzxcrtyfghvbnuiojklm,.p[];'/1234567689   53742y52734805"),
            LoginHasher::Hash("раводлыофврадцукенщцшгукрповмтоывмтьлждьфылоывшщ15468768   "),
            payload
        };
        auto [buffer, size] = MessageSerializer::MessageToBuffer(message);
        Message post_message{MessageDeserializer::MessageFromBuffer(buffer.get(), size)};
        REQUIRE(message == post_message);
    }

    SECTION("small buffer size") {
        Message message {
            1,
            LoginHasher::Hash("user1"),
            LoginHasher::Hash("user2"),
            payload
        };
        auto [buffer, size] = MessageSerializer::MessageToBuffer(message);
        REQUIRE_THROWS(MessageDeserializer::MessageFromBuffer(buffer.get(), size - 1));
    }
}