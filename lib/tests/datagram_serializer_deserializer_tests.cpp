#include <catch2/catch.hpp>

#include "../src/datagram_serializer.h"
#include "../src/datagram_deserializer.h"
#include "datagram_utils.h"

TEST_CASE("Datagram Serialization & Deserialization") {

    Message message {
        "127.0.0.1",
        1700000000,
        "hello"
    };

    SECTION("simple") {
        Datagram datagram {
            1,
            "user1",
            "user2",
            message
        };
        Buffer buffer{DatagramSerializer::DatagramToBuffer(datagram)};
        Datagram post_datagram{DatagramDeserializer::DatagramFromBuffer(buffer)};
        REQUIRE(datagram == post_datagram);
    }

    SECTION("64 byte sized logins") {
        Datagram datagram {
            1,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
            message
        };
        Buffer buffer{DatagramSerializer::DatagramToBuffer(datagram)};
        Datagram post_datagram{DatagramDeserializer::DatagramFromBuffer(buffer)};
        REQUIRE(datagram == post_datagram);
    }

    SECTION("big id") {
        Datagram datagram {
            999999999999999,
            "user1",
            "user2",
            message
        };
        Buffer buffer{DatagramSerializer::DatagramToBuffer(datagram)};
        Datagram post_datagram{DatagramDeserializer::DatagramFromBuffer(buffer)};
        REQUIRE(datagram == post_datagram);
    }

    SECTION("big everything") {
        Message long_message {
            "127.0.0.1",
            1700000000,
            "hellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohellohello"
        };
        Datagram datagram {
            123123123123123,
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
            long_message
        };
        Buffer buffer{DatagramSerializer::DatagramToBuffer(datagram)};
        Datagram post_datagram{DatagramDeserializer::DatagramFromBuffer(buffer)};
        REQUIRE(datagram == post_datagram);
    }

    SECTION("special symbols logins") {
        Datagram datagram {
            1,
            "qweasdzxcrtyfghvbnuiojklm,.p[];'/1234567689   53742y52734805",
            "раводлыофврадцукенщцшгукрповмтоывмтьлждьфылоывшщ15468768   ",
            message
        };
        Buffer buffer{DatagramSerializer::DatagramToBuffer(datagram)};
        Datagram post_datagram{DatagramDeserializer::DatagramFromBuffer(buffer)};
        REQUIRE(datagram == post_datagram);
    }
}
