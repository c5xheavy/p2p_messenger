#include <catch2/catch.hpp>
#include <opendht.h>

#include "../src/message.h"
#include "../src/signed_message.h"
#include "../src/message_serializer.h"
#include "../src/message_deserializer.h"
#include "message_utils.h"
#include "signed_message_utils.h"

TEST_CASE("Signed Message Serialization & Deserialization") {

    Payload payload {
        1700000000,
        "hello"
    };

    SECTION("signing and verifying encrypted message") {
        dht::crypto::Identity identity{dht::crypto::generateIdentity()};
        Message message {
            1,
            "user1",
            identity.first->getSharedPublicKey()->toString(),
            "user2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity.first->getSharedPublicKey())};
        SignedMessage signed_message {
            buffer,
            MessageSerializer::sign(buffer, identity.first)
        };
        buffer = MessageSerializer::signed_message_to_buffer(signed_message);
        SignedMessage post_signed_message{MessageDeserializer::signed_message_from_buffer(buffer)};
        REQUIRE(signed_message == post_signed_message);
        Message post_message{MessageDeserializer::message_from_buffer(signed_message.message, identity.first)};
        REQUIRE(message == post_message);
        REQUIRE(MessageDeserializer::check_signature(signed_message, std::make_shared<dht::crypto::PublicKey>(message.source_public_key)));
    }

    SECTION("signing and verifying non-encrypted message") {
        dht::crypto::Identity identity{dht::crypto::generateIdentity()};
        Message message {
            1,
            "user1",
            identity.first->getSharedPublicKey()->toString(),
            "user2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message)};
        SignedMessage signed_message {
            buffer,
            MessageSerializer::sign(buffer, identity.first)
        };
        buffer = MessageSerializer::signed_message_to_buffer(signed_message);
        SignedMessage post_signed_message{MessageDeserializer::signed_message_from_buffer(buffer)};
        REQUIRE(signed_message == post_signed_message);
        Message post_message{MessageDeserializer::message_from_buffer(signed_message.message)};
        REQUIRE(message == post_message);
        REQUIRE(MessageDeserializer::check_signature(signed_message, std::make_shared<dht::crypto::PublicKey>(message.source_public_key)));
    }

    SECTION("false signing and verifying encrypted message") {
        dht::crypto::Identity identity1{dht::crypto::generateIdentity()};
        Message message {
            1,
            "user1",
            identity1.first->getSharedPublicKey()->toString(),
            "user2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, identity1.first->getSharedPublicKey())};
        SignedMessage signed_message {
            buffer,
            MessageSerializer::sign(buffer, identity1.first)
        };
        buffer = MessageSerializer::signed_message_to_buffer(signed_message);
        SignedMessage post_signed_message{MessageDeserializer::signed_message_from_buffer(buffer)};
        REQUIRE(signed_message == post_signed_message);
        Message post_message{MessageDeserializer::message_from_buffer(signed_message.message, identity1.first)};
        REQUIRE(message == post_message);
        dht::crypto::Identity identity2{dht::crypto::generateIdentity()};
        REQUIRE(!MessageDeserializer::check_signature(signed_message, identity2.first->getSharedPublicKey()));
    }

    SECTION("false signing and verifying non-encrypted message") {
        dht::crypto::Identity identity1{dht::crypto::generateIdentity()};
        Message message {
            1,
            "user1",
            identity1.first->getSharedPublicKey()->toString(),
            "user2",
            payload
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message)};
        SignedMessage signed_message {
            buffer,
            MessageSerializer::sign(buffer, identity1.first)
        };
        buffer = MessageSerializer::signed_message_to_buffer(signed_message);
        SignedMessage post_signed_message{MessageDeserializer::signed_message_from_buffer(buffer)};
        REQUIRE(signed_message == post_signed_message);
        Message post_message{MessageDeserializer::message_from_buffer(signed_message.message)};
        REQUIRE(message == post_message);
        dht::crypto::Identity identity2{dht::crypto::generateIdentity()};
        REQUIRE(!MessageDeserializer::check_signature(signed_message, identity2.first->getSharedPublicKey()));
    }
}
