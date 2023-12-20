#include <iostream>

#include "payload.h"
#include "payload_serializer.h"
#include "payload_deserializer.h"
#include "message.h"
#include "message_serializer.h"
#include "message_deserializer.h"
#include "login_hasher.h"
#include "static_file_ip_resolver.h"

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
        LoginHasher::Hash("Amir"),
        LoginHasher::Hash("Matvey"),
        pre_payload
    };
    auto [buffer, buffer_size] = MessageSerializer::MessageToBuffer(message);
    Message post_message = MessageDeserializer::MessageFromBuffer(buffer.get(), buffer_size);
    std::cout << post_message.id << '\n';
    std::cout << post_message.source_login_hash << '\n';
    std::cout << post_message.destination_login_hash << '\n';
    std::cout << post_message.payload.time << '\n';
    std::cout << post_message.payload.text << '\n';
    std::cout << "===============================================================================\n";
    message = {
        1000000000000000000,
        LoginHasher::Hash("asdfasdfasdf"),
        LoginHasher::Hash("hfjshlljhlaasasdasd"),
        pre_payload
    };
    std::tie(buffer, buffer_size) = MessageSerializer::MessageToBuffer(message);
    post_message = MessageDeserializer::MessageFromBuffer(buffer.get(), buffer_size);
    std::cout << post_message.id << '\n';
    std::cout << post_message.source_login_hash << '\n';
    std::cout << post_message.destination_login_hash << '\n';
    std::cout << post_message.payload.time << '\n';
    std::cout << post_message.payload.text << '\n';
    std::cout << "===============================================================================\n";
    std::cout << LoginHasher::Hash("amir") << '\n';
    std::cout << "===============================================================================\n";
    StaticFileIpResolver static_file_ip_resolver("../static/login_to_ip.txt");
    std::cout << static_file_ip_resolver.Resolve(LoginHasher::Hash("amir")).value_or("not found") << '\n';
    std::cout << static_file_ip_resolver.Resolve(LoginHasher::Hash("not a login")).value_or("not found") << '\n';
    std::cout << "===============================================================================\n";
    return 0;
}
