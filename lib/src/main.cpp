#include <iostream>

#include "payload.h"
#include "payload_serializer.h"
#include "payload_deserializer.h"
#include "message.h"
#include "message_serializer.h"
#include "message_deserializer.h"

int main() {
    std::cout << "===============================================================================\n";
    Payload pre_payload {
        1700000001,
        "hello"
    };
    json::object jsonPayload{PayloadSerializer::PayloadToJson(pre_payload)};
    Payload post_payload{PayloadDeserializer::PayloadFromJson(jsonPayload)};
    std::cout << jsonPayload << '\n';
    std::cout << post_payload.time << '\n';
    std::cout << post_payload.text << '\n';
    std::cout << "===============================================================================\n";
    Message message {
        1000000000000000000,
        "Amir",
        "Matvey",
        pre_payload
    };
    auto [buffer, buffer_size] = MessageSerializer::MessageToBuffer(message);
    Message post_message = MessageDeserializer::MessageFromBuffer(buffer.get(), buffer_size);
    std::cout << post_message.id << '\n';
    std::cout << post_message.source_login << '\n';
    std::cout << post_message.destination_login << '\n';
    std::cout << post_message.payload.time << '\n';
    std::cout << post_message.payload.text << '\n';
    std::cout << "===============================================================================\n";
    message = {
        1000000000000000000,
        "asdfasdfasdf",
        "hfjshlljhlaasasdasd",
        pre_payload
    };
    std::tie(buffer, buffer_size) = MessageSerializer::MessageToBuffer(message);
    post_message = MessageDeserializer::MessageFromBuffer(buffer.get(), buffer_size);
    std::cout << post_message.id << '\n';
    std::cout << post_message.source_login << '\n';
    std::cout << post_message.destination_login << '\n';
    std::cout << post_message.payload.time << '\n';
    std::cout << post_message.payload.text << '\n';
    std::cout << "===============================================================================\n";
    return 0;
}
