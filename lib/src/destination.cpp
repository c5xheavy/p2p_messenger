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
    std::size_t max_buffer_size = 65000;

    try {
        net::io_context io_context;
        udp::socket socket{io_context, udp::endpoint{udp::v4(), port}};

        while (true) {
            std::shared_ptr<char[]> buffer{new char[max_buffer_size]};

            udp::endpoint remote_endpoint;
            std::size_t buffer_size = socket.receive_from(net::buffer(buffer.get(), max_buffer_size), remote_endpoint);
            socket.send_to(net::buffer("OK"), remote_endpoint);

            Message message = MessageDeserializer::MessageFromBuffer(buffer.get(), buffer_size);

            std::cout << message.id << '\n';
            std::cout << message.source_login << '\n';
            std::cout << message.destination_login << '\n';
            std::cout << message.payload.time << '\n';
            std::cout << message.payload.text << '\n';
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
