#include "message_receiver.h"

#include <cstdint>
#include <functional>
#include <iostream>
#include <syncstream>

#include <boost/asio.hpp>
#include <boost/system.hpp>
#include <opendht.h>

#include "message.h"
#include "message_deserializer.h"

namespace net = boost::asio;
using net::ip::udp;
namespace sys = boost::system;

MessageReceiver::MessageReceiver(udp::socket& socket, std::shared_ptr<dht::crypto::PrivateKey> private_key, MetadataIpResolver& metadata_ip_resolver, ReceiveMessageHandler handler)
    : socket_{socket}
    , private_key_{private_key}
    , metadata_ip_resolver_{metadata_ip_resolver}
    , handler_{handler} {
    async_wait();
}

MessageReceiver::~MessageReceiver() {    
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "MessageReceiver destructor called" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Closing MessageReceiver::socket_" << std::endl;
    socket_.close();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Closed MessageReceiver::socket_" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "MessageReceiver destructor finished" << std::endl;
}

void MessageReceiver::async_wait() {
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Waiting for incoming message" << std::endl;
    socket_.async_wait(udp::socket::wait_read, std::bind(&MessageReceiver::async_wait_handler, this, std::placeholders::_1));
}

void MessageReceiver::async_wait_handler(const sys::error_code& ec) {
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Got message" << std::endl;
    if (!ec) {
        std::vector<uint8_t> buffer(socket_.available());

        udp::endpoint remote_endpoint;
        size_t bytes_received{socket_.receive_from(net::buffer(buffer), remote_endpoint)};

        if (buffer.size() != bytes_received) {
            throw std::logic_error{"Bytes available is not equal bytes read"};
        }

        SignedMessage signed_message{MessageDeserializer::signed_message_from_buffer(buffer)};
        Message message{MessageDeserializer::message_from_buffer(signed_message.message, private_key_)};
        if (MessageDeserializer::check_signature(signed_message, std::make_shared<dht::crypto::PublicKey>(message.source_public_key))) {
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Received message:" << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.id << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.source_login << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.destination_login << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.payload.time << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.payload.text << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "End of message" << std::endl;

            metadata_ip_resolver_.put(message.source_login, std::make_shared<dht::crypto::PublicKey>(message.source_public_key), remote_endpoint.address().to_string(), remote_endpoint.port());
            handler_(std::move(message));
        }
    } else {
        std::osyncstream(std::cerr) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Wait message error: " << ec.what() << std::endl;
    }

    async_wait();
}
