#include <catch2/catch.hpp>

#include "../src/message_serializer.h"
#include "../src/message_deserializer.h"
#include "message_utils.h"

TEST_CASE("Message With Not Decrypted Payload Serialization & Deserialization") {

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
            "user2",
            "public_key2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity.first->getSharedPublicKey())};
        Message post_message{MessageDeserializer::message_with_not_decrypted_payload_from_buffer(buffer)};
        REQUIRE(message.id == post_message.id);
        REQUIRE(message.source_ip == post_message.source_ip);
        REQUIRE(message.source_port == post_message.source_port);
        REQUIRE(message.source_login == post_message.source_login);
        REQUIRE(message.source_public_key == post_message.source_public_key);
        REQUIRE(message.destination_login == post_message.destination_login);
        REQUIRE(message.destination_public_key == post_message.destination_public_key);
    }

    SECTION("64 byte sized logins") {
        Message message {
            1,
            "127.0.0.1",
            3001,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "public_key1",
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
            "public_key2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity.first->getSharedPublicKey())};
        Message post_message{MessageDeserializer::message_with_not_decrypted_payload_from_buffer(buffer)};
        REQUIRE(message.id == post_message.id);
        REQUIRE(message.source_ip == post_message.source_ip);
        REQUIRE(message.source_port == post_message.source_port);
        REQUIRE(message.source_login == post_message.source_login);
        REQUIRE(message.source_public_key == post_message.source_public_key);
        REQUIRE(message.destination_login == post_message.destination_login);
        REQUIRE(message.destination_public_key == post_message.destination_public_key);
    }

    SECTION("big id") {
        Message message {
            999999999999999,
            "127.0.0.1",
            3001,
            "user1",
            "public_key1",
            "user2",
            "public_key2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity.first->getSharedPublicKey())};
        Message post_message{MessageDeserializer::message_with_not_decrypted_payload_from_buffer(buffer)};
        REQUIRE(message.id == post_message.id);
        REQUIRE(message.source_ip == post_message.source_ip);
        REQUIRE(message.source_port == post_message.source_port);
        REQUIRE(message.source_login == post_message.source_login);
        REQUIRE(message.source_public_key == post_message.source_public_key);
        REQUIRE(message.destination_login == post_message.destination_login);
        REQUIRE(message.destination_public_key == post_message.destination_public_key);
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
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
            "public_key2",
            long_payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity.first->getSharedPublicKey())};
        Message post_message{MessageDeserializer::message_with_not_decrypted_payload_from_buffer(buffer)};
        REQUIRE(message.id == post_message.id);
        REQUIRE(message.source_ip == post_message.source_ip);
        REQUIRE(message.source_port == post_message.source_port);
        REQUIRE(message.source_login == post_message.source_login);
        REQUIRE(message.source_public_key == post_message.source_public_key);
        REQUIRE(message.destination_login == post_message.destination_login);
        REQUIRE(message.destination_public_key == post_message.destination_public_key);
    }

    SECTION("special symbols logins") {
        Message message {
            1,
            "127.0.0.1",
            3001,
            "qweasdzxcrtyfghvbnuiojklm,.p[];'/1234567689   53742y52734805",
            "public_key1",
            "раводлыофврадцукенщцшгукрповмтоывмтьлждьфылоывшщ15468768   ",
            "public_key2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity.first->getSharedPublicKey())};
        Message post_message{MessageDeserializer::message_with_not_decrypted_payload_from_buffer(buffer)};
        REQUIRE(message.id == post_message.id);
        REQUIRE(message.source_ip == post_message.source_ip);
        REQUIRE(message.source_port == post_message.source_port);
        REQUIRE(message.source_login == post_message.source_login);
        REQUIRE(message.source_public_key == post_message.source_public_key);
        REQUIRE(message.destination_login == post_message.destination_login);
        REQUIRE(message.destination_public_key == post_message.destination_public_key);
    }
}
