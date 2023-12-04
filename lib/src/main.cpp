#include <iostream>

#include "message.h"
#include "message_serializer.h"
#include "message_deserializer.h"

int main() {
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
    return 0;
}
