#include <catch2/catch.hpp>
#include <opendht.h>

#include "../src/message.h"
#include "../src/message_serializer.h"
#include "../src/message_deserializer.h"
#include "message_utils.h"

TEST_CASE("Encrypted Message Serialization & Deserialization") {

    dht::crypto::Identity source_identity{dht::crypto::generateIdentity()};
    dht::crypto::Identity destination_identity{dht::crypto::generateIdentity()};

    Payload payload {
        1700000000,
        "hello"
    };

    SECTION("encrypting and decrypting with the same RSA key pair") {
        Message message {
            1,
            "127.0.0.1",
            3001,
            "user1",
            source_identity.first->getSharedPublicKey()->toString(),
            "user2",
            destination_identity.first->getSharedPublicKey()->toString(),
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, destination_identity.first->getSharedPublicKey())};
        Message post_message{MessageDeserializer::message_from_buffer(buffer, destination_identity.first)};
        REQUIRE(message == post_message);
    }

    SECTION("encrypting and false decrypting with wrong RSA key pair") {
        Message message {
            1,
            "127.0.0.1",
            3001,
            "user1",
            source_identity.first->getSharedPublicKey()->toString(),
            "user2",
            destination_identity.first->getSharedPublicKey()->toString(),
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, source_identity.first->getSharedPublicKey())};
        REQUIRE_THROWS(MessageDeserializer::message_from_buffer(buffer, destination_identity.first));
    }

    SECTION("not encrypting and then decrypting") {
        Message message {
            1,
            "127.0.0.1",
            3001,
            "user1",
            source_identity.first->getSharedPublicKey()->toString(),
            "user2",
            destination_identity.first->getSharedPublicKey()->toString(),
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message)};
        REQUIRE_THROWS(MessageDeserializer::message_from_buffer(buffer, destination_identity.first));
    }
}
