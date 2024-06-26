#include <catch2/catch.hpp>

#include "../src/message_serializer.h"
#include "../src/message_deserializer.h"
#include "message_utils.h"

TEST_CASE("Message Serialization & Deserialization") {

    dht::crypto::Identity identity{dht::crypto::generateIdentity()};

    Payload payload {
        1700000000,
        "hello"
    };

    SECTION("simple") {
        Message message {
            1,
            "127.0.0.1",
            3001,
            "user1",
            "public_key1",
            "127.0.0.1",
            4001,
            "user2",
            "public_key2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity.first->getSharedPublicKey())};
        Message post_message{MessageDeserializer::message_from_buffer(buffer, identity.first)};
        REQUIRE(message == post_message);
    }

    SECTION("64 byte sized logins") {
        Message message {
            1,
            "127.0.0.1",
            3001,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "public_key1",
            "127.0.0.1",
            4001,
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
            "public_key2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity.first->getSharedPublicKey())};
        Message post_message{MessageDeserializer::message_from_buffer(buffer, identity.first)};
        REQUIRE(message == post_message);
    }

    SECTION("big id") {
        Message message {
            999999999999999,
            "127.0.0.1",
            3001,
            "user1",
            "public_key1",
            "127.0.0.1",
            4001,
            "user2",
            "public_key2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity.first->getSharedPublicKey())};
        Message post_message{MessageDeserializer::message_from_buffer(buffer, identity.first)};
        REQUIRE(message == post_message);
    }

    SECTION("big everything") {
        Payload long_payload {
            1700000000,
            "hellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohello"
        };
        Message message {
            123123123123123,
            "127.0.0.1",
            3001,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "public_key1",
            "127.0.0.1",
            4001,
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
            "public_key2",
            long_payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity.first->getSharedPublicKey())};
        Message post_message{MessageDeserializer::message_from_buffer(buffer, identity.first)};
        REQUIRE(message == post_message);
    }

    SECTION("special symbols logins") {
        Message message {
            1,
            "127.0.0.1",
            3001,
            "qweasdzxcrtyfghvbnuiojklm,.p[];'/1234567689   53742y52734805",
            "public_key1",
            "127.0.0.1",
            4001,
            "раводлыофврадцукенщцшгукрповмтоывмтьлждьфылоывшщ15468768   ",
            "public_key2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity.first->getSharedPublicKey())};
        Message post_message{MessageDeserializer::message_from_buffer(buffer, identity.first)};
        REQUIRE(message == post_message);
    }
}
