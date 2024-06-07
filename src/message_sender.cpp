#include "message_sender.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <syncstream>
#include <utility>

#include <boost/asio.hpp>

#include "dht_ip_resolver.h"
#include "message.h"
#include "signed_message.h"
#include "message_serializer.h"

namespace net = boost::asio;
using net::ip::udp;

MessageSender::MessageSender(udp::socket& socket, const std::string& source_ip, uint16_t source_port,
                             const std::string& source_login, const dht::crypto::Identity& identity,
                             const std::string& relay_node_ip, uint16_t relay_node_port,
                             SendMessageHandler handler)
    : socket_{socket}
    , source_ip_{source_ip}
    , source_port_{source_port}
    , source_login_{source_login}
    , identity_{identity}
    , relay_node_ip_{relay_node_ip}
    , relay_node_port_{relay_node_port}
    , handler_{handler} {
}

MessageSender::~MessageSender() {
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "MessageSender destructor called" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Closing MessageSender::socket_" << std::endl;
    socket_.close();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Closed MessageSender::socket_" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "MessageSender destructor finished" << std::endl;
}

void MessageSender::send_message(const std::string& destination_address, const std::string& destination_login,
                                 std::shared_ptr<dht::crypto::PublicKey> public_key, const std::string& text) {
    try {
        auto [destination_ip, destination_port]{get_ip_and_port_from_address(destination_address)};

        Message message {
            1,
            source_ip_,
            source_port_,
            source_login_,
            identity_.first->getSharedPublicKey()->toString(),
            destination_ip,
            destination_port,
            destination_login,
            public_key->toString(),
            {
                1700000000,
                text
            }
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message, public_key)};
        SignedMessage signed_message {
            buffer,
            MessageSerializer::sign(buffer, identity_.first)
        };
        buffer = MessageSerializer::signed_message_to_buffer(signed_message);

        udp::endpoint endpoint{net::ip::make_address(destination_ip), destination_port};
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Send message to " << destination_ip << ':' << destination_port << std::endl;
        socket_.send_to(net::buffer(buffer), endpoint);
        udp::endpoint relay_endpoint{net::ip::make_address(relay_node_ip_), relay_node_port_};
        socket_.send_to(net::buffer(buffer), endpoint);
        handler_(std::move(message));
    } catch (std::exception& e) {
        std::cerr << "Exception in send_message: " << e.what() << std::endl;
    }
}

std::pair<std::string, uint16_t> MessageSender::get_ip_and_port_from_address(const std::string& address) {
    size_t pos = address.find(':');
    if (pos == std::string::npos) {
        throw std::invalid_argument{"Invalid address"};
    }
    std::string ip{address.substr(0, pos)};
    uint16_t port = std::stoi(address.substr(pos + 1));
    return {ip, port};
}