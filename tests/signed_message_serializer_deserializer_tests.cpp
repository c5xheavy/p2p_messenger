#include <catch2/catch.hpp>
#include <opendht.h>

#include "../src/message.h"
#include "../src/signed_message.h"
#include "../src/message_serializer.h"
#include "../src/message_deserializer.h"
#include "message_utils.h"
#include "signed_message_utils.h"

TEST_CASE("Signed Message Serialization & Deserialization") {

    dht::crypto::Identity source_identity{dht::crypto::generateIdentity()};
    dht::crypto::Identity destination_identity{dht::crypto::generateIdentity()};

    Payload payload {
        1700000000,
        "hello"
    };

    SECTION("signing and verifying encrypted message") {
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
        SignedMessage signed_message {
            buffer,
            MessageSerializer::sign(buffer, source_identity.first)
        };
        buffer = MessageSerializer::signed_message_to_buffer(signed_message);
        SignedMessage post_signed_message{MessageDeserializer::signed_message_from_buffer(buffer)};
        REQUIRE(signed_message == post_signed_message);
        Message post_message{MessageDeserializer::message_from_buffer(signed_message.message, destination_identity.first)};
        REQUIRE(message == post_message);
        REQUIRE(MessageDeserializer::check_signature(signed_message, std::make_shared<dht::crypto::PublicKey>(message.source_public_key)));
    }

    SECTION("false signing and verifying encrypted message") {
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
        SignedMessage signed_message {
            buffer,
            MessageSerializer::sign(buffer, source_identity.first)
        };
        buffer = MessageSerializer::signed_message_to_buffer(signed_message);
        SignedMessage post_signed_message{MessageDeserializer::signed_message_from_buffer(buffer)};
        REQUIRE(signed_message == post_signed_message);
        Message post_message{MessageDeserializer::message_from_buffer(signed_message.message, destination_identity.first)};
        REQUIRE(message == post_message);
        REQUIRE(!MessageDeserializer::check_signature(signed_message, destination_identity.first->getSharedPublicKey()));
    }
}
