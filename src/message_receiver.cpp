#include "message_receiver.h"

#include <cstdint>
#include <fstream>
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

MessageReceiver::MessageReceiver(udp::socket& socket, std::shared_ptr<dht::crypto::PrivateKey> private_key, MetadataIpResolver& metadata_ip_resolver,
                                bool relay, UdpHolePuncher& upd_hole_puncher, ReceiveMessageHandler handler)
    : socket_{socket}
    , private_key_{private_key}
    , metadata_ip_resolver_{metadata_ip_resolver}
    , relay_{relay}
    , upd_hole_puncher_{upd_hole_puncher}
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
        
        ofs << "received buffer" << std::endl;
        try {
            SignedMessage signed_message{MessageDeserializer::signed_message_from_buffer(buffer)};
            ofs << "signed_message_from_buffer" << std::endl;
            if (MessageDeserializer::destinantion_public_key_from_buffer(signed_message.message) == private_key_->getSharedPublicKey()->toString()) {
                ofs << "Received message" << std::endl;
                try {
                    Message message{MessageDeserializer::message_from_buffer(signed_message.message, private_key_)};
                    if (MessageDeserializer::check_signature(signed_message, std::make_shared<dht::crypto::PublicKey>(message.source_public_key))) {
                        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Received message:" << std::endl;
                        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.id << std::endl;
                        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.source_login << std::endl;
                        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.destination_login << std::endl;
                        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.payload.time << std::endl;
                        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.payload.text << std::endl;
                        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "End of message" << std::endl;

                        if (message.source_ip == remote_endpoint.address().to_string()) {
                            metadata_ip_resolver_.put(message.source_login, std::make_shared<dht::crypto::PublicKey>(message.source_public_key), remote_endpoint.address().to_string(), remote_endpoint.port());
                            upd_hole_puncher_.start_hole_punching(remote_endpoint.address().to_string(), remote_endpoint.port());
                        } else {
                            metadata_ip_resolver_.put(message.source_login, std::make_shared<dht::crypto::PublicKey>(message.source_public_key), message.source_ip, message.source_port);
                            upd_hole_puncher_.start_hole_punching(message.source_ip, message.source_port);
                        }
                        handler_(std::move(message));
                    }
                } catch(std::exception& e) {
                    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << e.what() << std::endl;
                }
            } else {
                ofs << "Received someone else's message" << std::endl;
                Message message{MessageDeserializer::message_with_not_decrypted_payload_from_buffer(signed_message.message)};
                if (MessageDeserializer::check_signature(signed_message, std::make_shared<dht::crypto::PublicKey>(message.source_public_key))) {
                    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Received someone else's message:" << std::endl;
                    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.id << std::endl;
                    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.source_login << std::endl;
                    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.destination_login << std::endl;
                    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.payload.time << std::endl;
                    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.payload.text << std::endl;
                    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "End of message" << std::endl;

                    if (relay_) {
                        ofs << "relaying" << std::endl;
                        try {
                            std::optional<std::string> address = metadata_ip_resolver_.resolve(message.destination_login, std::make_shared<dht::crypto::PublicKey>(message.source_public_key)->getId());
                            if (address) {
                                ofs << "resolved with metadata" << std::endl;
                                size_t pos = (*address).find(':');
                                if (pos == std::string::npos) {
                                    throw std::invalid_argument{"Invalid address"};
                                }
                                std::string ip{(*address).substr(0, pos)};
                                uint16_t port = std::stoi((*address).substr(pos + 1));
                                ofs << "relaying to ip: " << ip << ", port: " << port << std::endl;
                                udp::endpoint endpoint{net::ip::make_address(ip), port};
                                socket_.send_to(net::buffer(buffer), endpoint);
                            } else {
                                udp::endpoint endpoint{net::ip::make_address(message.destination_ip), message.destination_port};
                                socket_.send_to(net::buffer(buffer), endpoint);
                            }
                        } catch (std::exception& e) {
                            std::cerr << "Exception in send_message: " << e.what() << std::endl;
                        }
                    }
                }
            }
        } catch(std::exception&) {
            ofs << "received udp hole punch" << std::endl;
        }
    } else {
        std::osyncstream(std::cerr) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Wait message error: " << ec.what() << std::endl;
    }

    async_wait();
}
