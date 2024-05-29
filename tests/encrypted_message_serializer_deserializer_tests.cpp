#include <catch2/catch.hpp>
#include <opendht.h>

#include "../src/message.h"
#include "../src/message_serializer.h"
#include "../src/message_deserializer.h"
#include "message_utils.h"

TEST_CASE("Encrypted Message Serialization & Deserialization") {

    Payload payload {
        1700000000,
        "hello"
    };

    SECTION("encrypting and decrypting with the same RSA key pair") {
        dht::crypto::Identity identity{dht::crypto::generateIdentity()};
        Message message {
            1,
            "127.0.0.1",
            3001,
            "user1",
            identity.first->getSharedPublicKey()->toString(),
            "user2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity.first->getSharedPublicKey())};
        Message post_message{MessageDeserializer::message_from_buffer(buffer, identity.first)};
        REQUIRE(message == post_message);
    }

    SECTION("no encrypting or decrypting") {
        Message message {
            1,
            "127.0.0.1",
            3001,
            "user1",
            "public_key1",
            "user2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message)};
        Message post_message{MessageDeserializer::message_from_buffer(buffer)};
        REQUIRE(message == post_message);
    }

    SECTION("encrypting and then not decrypting") {
        dht::crypto::Identity identity{dht::crypto::generateIdentity()};
        Message message {
            1,
            "127.0.0.1",
            3001,
            "user1",
            identity.first->getSharedPublicKey()->toString(),
            "user2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity.first->getSharedPublicKey())};
        REQUIRE_THROWS(MessageDeserializer::message_from_buffer(buffer));
    }

    SECTION("not encrypting and then decrypting") {
        dht::crypto::Identity identity{dht::crypto::generateIdentity()};
        Message message {
            1,
            "127.0.0.1",
            3001,
            "user1",
            identity.first->getSharedPublicKey()->toString(),
            "user2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message)};
        REQUIRE_THROWS(MessageDeserializer::message_from_buffer(buffer, identity.first));
    }
}
