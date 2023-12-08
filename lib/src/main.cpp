#include <iostream>

#include "message.h"
#include "message_serializer.h"
#include "message_deserializer.h"
#include "datagram.h"
#include "datagram_serializer.h"
#include "datagram_deserializer.h"

int main() {
    std::cout << "===============================================================================\n";
    Message pre_message {
        "127.0.0.1",
        1700000001,
        "hello"
    };
    json::object jsonMessage{MessageSerializer::MessageToJson(pre_message)};
    Message post_message{MessageDeserializer::MessageFromJson(jsonMessage)};
    std::cout << jsonMessage << '\n';
    std::cout << post_message.sender_ip << '\n';
    std::cout << post_message.time << '\n';
    std::cout << post_message.text << '\n';
    std::cout << "===============================================================================\n";
    Datagram datagram {
        1000000000000000000,
        "Amir",
        "Matvey",
        pre_message
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
    std::cout << post_datagram.message.sender_ip << '\n';
    std::cout << post_datagram.message.time << '\n';
    std::cout << post_datagram.message.text << '\n';
    std::cout << "===============================================================================\n";
    datagram = {
        1000000000000000000,
        "asdfasdfasdf",
        "hfjshlljhlaasasdasd",
        pre_message
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
    std::cout << post_datagram.message.sender_ip << '\n';
    std::cout << post_datagram.message.time << '\n';
    std::cout << post_datagram.message.text << '\n';
    std::cout << "===============================================================================\n";
    return 0;
}
