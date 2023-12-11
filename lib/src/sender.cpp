#include <array>
#include <iostream>
#include <string>
#include <vector>

#include <boost/asio.hpp>

#include "datagram_serializer.h"

namespace net = boost::asio;
using net::ip::udp;

int main(int argc, const char** argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <IP>  <port>" << std::endl;
        return 1;
    }

    std::string ip = argv[1];
    std::uint16_t port = std::stoi(argv[2]);

    try {
        Datagram datagram {
            1,
            "sender",
            "recipient",
            {
                "127.0.0.1",
                1700000000,
                "hello"
            }
        };
        std::vector<std::uint8_t> buffer{DatagramSerializer::DatagramToBuffer(datagram)};

        net::io_context io_context;
        udp::socket socket{io_context, udp::v4()};
        udp::endpoint endpoint{net::ip::make_address(ip), port};
        socket.send_to(net::buffer(buffer), endpoint);

        std::array<char, 128> recv_buf;
        udp::endpoint remote_endpoint;
        std::size_t size = socket.receive_from(net::buffer(recv_buf), remote_endpoint);
        std::cout.write(recv_buf.data(), size) << '\n';
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
