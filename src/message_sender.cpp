#include "message_sender.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <syncstream>
#include <utility>

#include <boost/asio.hpp>

#include "dht_ip_resolver.h"
#include "message.h"
#include "message_serializer.h"

namespace net = boost::asio;
using net::ip::udp;

MessageSender::MessageSender(net::io_context& io_context, DhtIpResolver& dht_ip_resolver, const std::string& source_login, SendMessageHandler handler)
    : io_context_{io_context}
    , dht_ip_resolver_{dht_ip_resolver}
    , source_login_{source_login}
    , socket_{io_context, udp::v4()}
    , handler_{handler} {
}

MessageSender::~MessageSender() {
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "MessageSender destructor called" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Closing MessageSender::socket_" << std::endl;
    socket_.close();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Closed MessageSender::socket_" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "MessageSender destructor finished" << std::endl;
}

void MessageSender::send_message(const std::string& destination_login, const dht::InfoHash& public_key_id, const std::string& text) {
    try {
        Message message {
            1,
            source_login_,
            destination_login,
            {
                1700000000,
                text
            }
        };
        std::vector<uint8_t> buffer{MessageSerializer::message_to_buffer(message)};

        std::optional<std::string> destination_address = dht_ip_resolver_.resolve(destination_login, public_key_id);
        if (!destination_address) {
            throw std::logic_error{"Destination address is not set"};
        }
        auto [destination_ip, destination_port]{get_ip_and_port_from_address(*destination_address)};

        udp::endpoint endpoint{net::ip::make_address(destination_ip), destination_port};
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Send message to " << destination_ip << ':' << destination_port << std::endl;
        socket_.send_to(net::buffer(buffer), endpoint);
        handler_(source_login_, text);
    } catch (std::exception& e) {
        std::cerr << "Exception in send_message: " << e.what() << std::endl;
    }
}

std::pair<std::string, std::uint16_t> MessageSender::get_ip_and_port_from_address(const std::string& address) {
    std::size_t pos = address.find(':');
    if (pos == std::string::npos) {
        throw std::invalid_argument{"Invalid address"};
    }
    std::string ip{address.substr(0, pos)};
    std::uint16_t port = std::stoi(address.substr(pos + 1));
    return {ip, port};
}