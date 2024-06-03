#include "p2p_messenger_impl.h"

#include <iostream>
#include <memory>
#include <string>
#include <syncstream>
#include <thread>
#include <utility>


P2PMessengerImpl::P2PMessengerImpl(const std::string& my_login, uint16_t dht_port,
                                   const std::string& my_ip, uint16_t my_port,
                                   bool generate_crypto_identity, const std::string& crypto_identity_path,
                                   MessageSender::SendMessageHandler&& send_message_handler,
                                   MessageReceiver::ReceiveMessageHandler&& receive_message_handler,
                                   DhtIpResolver::ListenLoginHandler&& listen_login_handler)
    : my_login_{my_login}
    , dht_port_{dht_port}
    , my_ip_{my_ip}
    , my_port_{my_port}
    , generate_crypto_identity_{generate_crypto_identity}
    , crypto_identity_path_{crypto_identity_path}
    , identity_{get_identity(generate_crypto_identity_, crypto_identity_path_)}
    , num_threads_{4}
    , threads_{}
    , io_context_{static_cast<int>(num_threads_)}
    , work_guard_{net::make_work_guard(io_context_)}
    , message_sender_{io_context_, dht_ip_resolver_, my_ip_, my_port_, my_login_, identity_, [send_message_handler = std::move(send_message_handler)](Message&& message) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Message sent" << std::endl;
        send_message_handler(std::move(message));
    }}
    , message_receiver_{io_context_, my_port_, identity_.first, [this, receive_message_handler = std::move(receive_message_handler), listen_login_handler](Message&& message) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Message received" << std::endl;
        metadata_ip_resolver_.put(message.source_login, std::make_shared<dht::crypto::PublicKey>(message.source_public_key), message.source_ip, message.source_port);
        listen_login_handler({message.source_login, dht::crypto::PublicKey{message.source_public_key}.getId()});
        receive_message_handler(std::move(message));
    }}
    , dht_ip_resolver_{io_context_, dht_port_, identity_, [listen_login_handler](Contact&& contact) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Contact received" << std::endl;
        listen_login_handler(std::move(contact));
    }}
    , metadata_ip_resolver_{} {
    // put data on the dht
    dht_ip_resolver_.put_signed(my_login_, my_ip_, my_port_);

    // listen for data on the dht
    dht_ip_resolver_.listen(my_login_);

    threads_.reserve(num_threads_);
    for (size_t i = 0; i < num_threads_; ++i) {
        threads_.emplace_back([this] {
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Running io_context thread" << std::endl;
            io_context_.run();
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopping io_context thread" << std::endl;
        });
    }
}

P2PMessengerImpl::~P2PMessengerImpl()
{
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "P2PMessengerImpl destructor called" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Shutting down gracefully..." << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopping work_guard" << std::endl;
    work_guard_.reset();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopped work_guard" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopping io_context" << std::endl;
    io_context_.stop();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopped io_context" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "P2PMessengerImpl destructor finished" << std::endl;
}

void P2PMessengerImpl::send_message(const std::string& login, const dht::InfoHash& public_key_id, const std::string& message) {
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "on_send_message called" << std::endl;
    if (login.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Login is empty" << std::endl;
        return;
    }
    if (message.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Message is empty" << std::endl;
        return;
    }
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Sending message to " << login << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Sending message: " << message << std::endl;
    net::post(io_context_, [this, login, public_key_id, message]() {{
            std::optional<std::string> destination_address = resolve(login, public_key_id);
            if (destination_address) {
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is set" << std::endl;
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Sending message to " << login << '\n';
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Calling send_message" << '\n';
                net::post(io_context_, [this, destination_address, login, public_key_id, message]() {
                    message_sender_.send_message(*destination_address, login, dht_ip_resolver_.get_public_key_by_public_key_id(public_key_id), message);
                });
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Called send_message" << '\n';
            } else {
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is not set" << std::endl;
            }
        }});
}

std::optional<std::string> P2PMessengerImpl::resolve(const std::string& login, const dht::InfoHash& public_key_id) {
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Searching for login: " << login << std::endl;
    std::optional<std::string> address = dht_ip_resolver_.resolve(login, public_key_id);
    if (address) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Found address: " << *address << std::endl;
        return address;
    }
    return metadata_ip_resolver_.resolve(login, public_key_id);
}

void P2PMessengerImpl::listen(const std::string& login) {
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "on_listen called" << std::endl;
    if (login.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Login is empty" << std::endl;
        return;
    }
    dht_ip_resolver_.listen(login);
}

dht::crypto::Identity P2PMessengerImpl::get_identity(bool generate_crypto_identity, const std::string& crypto_identity_path) {
    if (generate_crypto_identity) {
        dht::crypto::Identity identity = dht::crypto::generateIdentity();
        dht::crypto::saveIdentity(identity, crypto_identity_path);
        return identity;
    }
    return dht::crypto::loadIdentity(crypto_identity_path);
}
