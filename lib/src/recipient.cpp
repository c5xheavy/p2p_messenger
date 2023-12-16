#include <iostream>
#include <string>
#include <vector>

#include <boost/asio.hpp>

#include "message_deserializer.h"

namespace net = boost::asio;
using net::ip::udp;

int main(int argc, const char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    std::uint16_t port = std::stoi(argv[1]);
    std::size_t max_buffer_size = 1024;

    try {
        net::io_context io_context;
        udp::socket socket{io_context, udp::endpoint{udp::v4(), port}};

        while (true) {
            std::vector<std::uint8_t> buffer(max_buffer_size);  

            udp::endpoint remote_endpoint;
            std::size_t size = socket.receive_from(net::buffer(buffer), remote_endpoint);
            socket.send_to(net::buffer("Hello from recipient"), remote_endpoint);

            buffer.resize(size);
            buffer.shrink_to_fit();

            Message message = MessageDeserializer::MessageFromBuffer(buffer);

            std::cout << message.id << '\n';
            std::cout << message.recipient_login << '\n';
            std::cout << message.sender_login << '\n';
            std::cout << message.payload.time << '\n';
            std::cout << message.payload.text << '\n';
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
