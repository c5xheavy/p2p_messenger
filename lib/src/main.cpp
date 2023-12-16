#include <iostream>

#include "payload.h"
#include "payload_serializer.h"
#include "payload_deserializer.h"
#include "datagram.h"
#include "datagram_serializer.h"
#include "datagram_deserializer.h"

int main() {
    std::cout << "===============================================================================\n";
    Payload pre_payload {
        "127.0.0.1",
        1700000001,
        "hello"
    };
    json::object jsonPayload{PayloadSerializer::PayloadToJson(pre_payload)};
    Payload post_payload{PayloadDeserializer::PayloadFromJson(jsonPayload)};
    std::cout << jsonPayload << '\n';
    std::cout << post_payload.sender_ip << '\n';
    std::cout << post_payload.time << '\n';
    std::cout << post_payload.text << '\n';
    std::cout << "===============================================================================\n";
    Datagram datagram {
        1000000000000000000,
        "Amir",
        "Matvey",
        pre_payload
    };
    Buffer buffer = DatagramSerializer::DatagramToBuffer(datagram);
    std::cout << buffer.size() << '\n';
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        std::cout << static_cast<std::uint8_t>(buffer[i]);
    }
    std::cout << '\n';
    Datagram post_datagram = DatagramDeserializer::DatagramFromBuffer(buffer);
    std::cout << post_datagram.id << '\n';
    std::cout << post_datagram.sender_login << '\n';
    std::cout << post_datagram.recipient_login << '\n';
    std::cout << post_datagram.payload.sender_ip << '\n';
    std::cout << post_datagram.payload.time << '\n';
    std::cout << post_datagram.payload.text << '\n';
    std::cout << "===============================================================================\n";
    datagram = {
        1000000000000000000,
        "asdfasdfasdf",
        "hfjshlljhlaasasdasd",
        pre_payload
    };
    buffer = DatagramSerializer::DatagramToBuffer(datagram);
    std::cout << buffer.size() << '\n';
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        std::cout << static_cast<std::uint8_t>(buffer[i]);
    }
    std::cout << '\n';
    post_datagram = DatagramDeserializer::DatagramFromBuffer(buffer);
    std::cout << post_datagram.id << '\n';
    std::cout << post_datagram.sender_login << '\n';
    std::cout << post_datagram.recipient_login << '\n';
    std::cout << post_datagram.payload.sender_ip << '\n';
    std::cout << post_datagram.payload.time << '\n';
    std::cout << post_datagram.payload.text << '\n';
    std::cout << "===============================================================================\n";
    return 0;
}
