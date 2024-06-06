#include <catch2/catch.hpp>
#include <opendht.h>

#include "../src/message.h"
#include "../src/message_serializer.h"
#include "../src/message_deserializer.h"
#include "message_utils.h"

TEST_CASE("Destination public key from buffer") {

    dht::crypto::Identity source_identity{dht::crypto::generateIdentity()};
    dht::crypto::Identity destination_identity{dht::crypto::generateIdentity()};

    Payload payload {
        1700000000,
        "hello"
    };

    SECTION("destinantion public key from with correct receiver public key") {
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
        REQUIRE(MessageDeserializer::destinantion_public_key_from_buffer(buffer) == destination_identity.first->getSharedPublicKey()->toString());
    }

    SECTION("destinantion public key from with wrong receiver public key") {
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
        REQUIRE(MessageDeserializer::destinantion_public_key_from_buffer(buffer) != source_identity.first->getSharedPublicKey()->toString());
    }
}
