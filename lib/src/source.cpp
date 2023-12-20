#include <array>
#include <iostream>
#include <string>
#include <vector>

#include <boost/asio.hpp>

#include "message_serializer.h"
#include "login_hasher.h"

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
        Message message {
            1,
            LoginHasher::Hash("source"),
            LoginHasher::Hash("destination"),
            {
                1700000000,
                "hello"
            }
        };
        auto [buffer, buffer_size] = MessageSerializer::MessageToBuffer(message);

        net::io_context io_context;
        udp::socket socket{io_context, udp::v4()};
        udp::endpoint endpoint{net::ip::make_address(ip), port};
        socket.send_to(net::buffer(buffer.get(), buffer_size), endpoint);

        std::array<char, 128> recv_buf;
        std::size_t size = socket.receive(net::buffer(recv_buf));
        std::cout.write(recv_buf.data(), size) << '\n';
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
