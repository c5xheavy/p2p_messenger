#include <iostream>
#include <string>
#include <vector>

#include <boost/asio.hpp>

#include "datagram_deserializer.h"

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

            Datagram datagram = DatagramDeserializer::DatagramFromBuffer(buffer);

            std::cout << datagram.id << '\n';
            std::cout << datagram.sender_login << '\n';
            std::cout << datagram.recipient_login << '\n';
            std::cout << datagram.message.sender_ip << '\n';
            std::cout << datagram.message.time << '\n';
            std::cout << datagram.message.text << '\n';
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
