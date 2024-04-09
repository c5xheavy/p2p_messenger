#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/asio.hpp>
#include <opendht.h>

#include "address.h"
#include "login_hasher.h"
#include "message.h"
#include "message_serializer.h"
#include "message_deserializer.h"
#include "thread_safe_queue.h"

namespace net = boost::asio;
using net::ip::udp;

ThreadSafeQueue<std::string> messages;
Address destination_address;

std::atomic<bool> should_stop{false};


void receive_messages(net::io_context& io_context, std::uint16_t port) {
    try {
        udp::socket socket{io_context, udp::endpoint{udp::v4(), port}};
        while (!should_stop) { // does not stop right now
            socket.wait(net::ip::udp::socket::wait_read); // because of this

            std::size_t bytes_available{socket.available()};
            std::shared_ptr<char[]> buffer{new char[bytes_available]};

            udp::endpoint remote_endpoint;
            std::size_t buffer_size{socket.receive_from(net::buffer(buffer.get(), bytes_available), remote_endpoint)};
            socket.send_to(net::buffer("OK"), remote_endpoint);

            if (bytes_available != buffer_size) {
                throw std::logic_error{"Bytes available is not equal bytes read"};
            }

            Message message{MessageDeserializer::MessageFromBuffer(buffer.get(), buffer_size)};

            std::cout << "Received message:\n";
            std::cout << message.id << '\n';
            std::cout << message.source_login_hash << '\n';
            std::cout << message.destination_login_hash << '\n';
            std::cout << message.payload.time << '\n';
            std::cout << message.payload.text << '\n';
            std::cout << "End of message.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in receive_messages: " << e.what() << std::endl;
    }
}

void send_messages(net::io_context& io_context) {
    try {
        udp::socket socket{io_context, udp::v4()};
        while (!should_stop || !messages.empty()) { // also does not stop right now
            std::string text = messages.waitPop();
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

            std::string destination_ip;
            std::uint16_t destination_port;
            std::tie(destination_ip, destination_port) = destination_address.getWait(); // because of this
            udp::endpoint endpoint{net::ip::make_address(destination_ip), destination_port};
            std::cout << "Send message to " << destination_ip << ':' << destination_port << std::endl;
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
    if (argc != 6) {
        std::cout << "Usage: " << argv[0] << " <DHT port> <IP> <port> <your login> <destination login>" << std::endl;
        return 1;
    }

    std::uint16_t dht_port{static_cast<std::uint16_t>(std::stoi(argv[1]))};
    std::string my_ip{argv[2]};
    std::uint16_t my_port{static_cast<std::uint16_t>(std::stoi(argv[3]))};
    std::string my_login{argv[4]};
    std::string destination_login{argv[5]};

    net::io_context io_context;

    dht::DhtRunner node;
    
    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port.
    node.run(dht_port, dht::crypto::generateIdentity(), true);

    // Join the network through any running node,
    // here using a known bootstrap node.
    node.bootstrap("bootstrap.jami.net", "4222");

    // put data on the dht
    std::string my_address{my_ip + ":" + std::to_string(my_port)};
    node.put(dht::InfoHash::get(my_login), {(const std::uint8_t*)my_address.data(), my_address.size()});

    auto key = dht::InfoHash::get(destination_login);
    auto token = node.listen(key,
        [](const std::vector<std::shared_ptr<dht::Value>>& values, bool expired) {
            std::cout << "listen callback called" << std::endl;
            for (const auto& value : values) {
                std::cout << "Found value: " << *value << ", " << (expired ? "expired" : "added") << std::endl;
                std::string address;
                for (const char& x : value->data) {
                    address += x;
                }
                std::cout << "Found address: " << address << std::endl;
                if (!expired) {
                    destination_address.set(address);
                    std::cout << "Update destination address to " << address << std::endl;
                }
            }
            return true; // keep listening
        }
    );

    std::thread receive_thread{[&]() { receive_messages(io_context, my_port); }};
    std::thread send_thread{[&]() { send_messages(io_context); }};

    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "exit") {
            break;
        }
        messages.push(message);
    }

    should_stop = true;

    std::cout << "Joining threads..." << std::endl;
    send_thread.join();
    std::cout << "Send thread joined" << std::endl;
    receive_thread.join();
    std::cout << "Receive thread joined" << std::endl;
    node.cancelListen(key, std::move(token));
    std::cout << "Cancel listen" << std::endl;
    node.join();
    std::cout << "Node joined" << std::endl;

    return 0;
}
