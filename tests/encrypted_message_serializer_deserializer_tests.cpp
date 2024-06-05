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
        dht::crypto::Identity identity1{dht::crypto::generateIdentity()};
        dht::crypto::Identity identity2{dht::crypto::generateIdentity()};
        Message message {
            1,
            "127.0.0.1",
            3001,
            "user1",
            identity1.first->getSharedPublicKey()->toString(),
            "user2",
            identity2.first->getSharedPublicKey()->toString(),
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity2.first->getSharedPublicKey())};
        Message post_message{MessageDeserializer::message_from_buffer(buffer, identity2.first)};
        REQUIRE(message == post_message);
    }

    SECTION("encrypting and false decrypting with wrong RSA key pair") {
        dht::crypto::Identity identity1{dht::crypto::generateIdentity()};
        dht::crypto::Identity identity2{dht::crypto::generateIdentity()};
        Message message {
            1,
            "127.0.0.1",
            3001,
            "user1",
            identity1.first->getSharedPublicKey()->toString(),
            "user2",
            identity2.first->getSharedPublicKey()->toString(),
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity1.first->getSharedPublicKey())};
        REQUIRE_THROWS(MessageDeserializer::message_from_buffer(buffer, identity2.first));
    }

    SECTION("not encrypting and then decrypting") {
        dht::crypto::Identity identity1{dht::crypto::generateIdentity()};
        dht::crypto::Identity identity2{dht::crypto::generateIdentity()};
        Message message {
            1,
            "127.0.0.1",
            3001,
            "user1",
            identity1.first->getSharedPublicKey()->toString(),
            "user2",
            identity2.first->getSharedPublicKey()->toString(),
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message)};
        REQUIRE_THROWS(MessageDeserializer::message_from_buffer(buffer, identity2.first));
    }
}
