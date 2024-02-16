#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

#include <boost/asio.hpp>

#include "login_hasher.h"
#include "message.h"
#include "message_serializer.h"
#include "message_deserializer.h"
#include "payload.h"

namespace net = boost::asio;
using net::ip::udp;

void receive_messages(net::io_context& io_context, std::uint16_t port) {
    try {
        udp::socket socket{io_context, udp::endpoint{udp::v4(), port}};
        while (true) {
            socket.wait(net::ip::udp::socket::wait_read);

            std::size_t bytes_available{socket.available()};
            std::shared_ptr<char[]> buffer{new char[bytes_available]};

            udp::endpoint remote_endpoint;
            std::size_t buffer_size{socket.receive_from(net::buffer(buffer.get(), bytes_available), remote_endpoint)};
            socket.send_to(net::buffer("OK"), remote_endpoint);

            if (bytes_available != buffer_size) {
                throw std::logic_error("Bytes available is not equal bytes read");
            }

            Message message{MessageDeserializer::MessageFromBuffer(buffer.get(), buffer_size)};

            std::cout << message.id << '\n';
            std::cout << message.source_login_hash << '\n';
            std::cout << message.destination_login_hash << '\n';
            std::cout << message.payload.time << '\n';
            std::cout << message.payload.text << '\n';
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in receive_messages: " << e.what() << std::endl;
    }
}

void send_messages(net::io_context& io_context, const std::string& ip, std::uint16_t port) {
    try {
        udp::socket socket{io_context, udp::v4()};
        while (true) {
            std::string text;
            std::getline(std::cin, text);
            Message message {
                1,
                LoginHasher::Hash("source"),
                LoginHasher::Hash("destination"),
                {
                    1700000000,
                    text
                }
            };
            auto [buffer, buffer_size]{MessageSerializer::MessageToBuffer(message)};

            udp::endpoint endpoint{net::ip::make_address(ip), port};
            socket.send_to(net::buffer(buffer.get(), buffer_size), endpoint);

            std::array<char, 128> recv_buf;
            std::size_t size{socket.receive(net::buffer(recv_buf))};
            std::cout.write(recv_buf.data(), size) << '\n';
        }
    } catch (std::exception& e) {
        std::cerr << "Exception in send_messages: " << e.what() << std::endl;
    }
}

int main(int argc, const char** argv) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <receive port> <IP>  <send port>" << std::endl;
        return 1;
    }

    std::uint16_t receive_port{static_cast<std::uint16_t>(std::stoi(argv[1]))};
    std::string ip{argv[2]};
    std::uint16_t send_port{static_cast<std::uint16_t>(std::stoi(argv[3]))};

    net::io_context io_context;

    std::thread receive_thread([&]() { receive_messages(io_context, receive_port); });
    std::thread send_thread([&]() { send_messages(io_context, ip, send_port); });

    receive_thread.join();
    send_thread.join();

    return 0;
}
