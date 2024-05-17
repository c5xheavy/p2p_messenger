#include "p2p_messenger_impl.h"

#include <iostream>
#include <string>
#include <syncstream>
#include <thread>
#include <utility>


P2PMessengerImpl::P2PMessengerImpl(const std::string& my_login, std::uint16_t dht_port,
                                   const std::string& my_ip, std::uint16_t my_port,
                                   bool generate_crypto_identity, const std::string& crypto_identity_path,
                                   MessageSender::SendMessageHandler send_message_handler,
                                   MessageReceiver::ReceiveMessageHandler receive_message_handler,
                                   DhtIpResolver::ListenLoginHandler listen_login_handler)
    : dht_port_{dht_port}
    , my_ip_{my_ip}
    , my_port_{my_port}
    , my_login_{my_login}
    , generate_crypto_identity_{generate_crypto_identity}
    , crypto_identity_path_{crypto_identity_path}
    , num_threads_{4}
    , io_context_{static_cast<int>(num_threads_)}
    , work_guard_{net::make_work_guard(io_context_)}
    , dht_ip_resolver_{io_context_, dht_port_, generate_crypto_identity_, crypto_identity_path_, listen_login_handler}
    , threads_{}
    , message_sender_{io_context_, dht_ip_resolver_, my_login_, send_message_handler}
    , message_receiver_{io_context_, my_port_, receive_message_handler} {
    // put data on the dht
    dht_ip_resolver_.put_signed(my_login_, my_ip_, my_port_);

    // listen for data on the dht
    dht_ip_resolver_.listen(my_login_);

    threads_.reserve(num_threads_);
    for (std::size_t i = 0; i < num_threads_; ++i) {
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

void P2PMessengerImpl::send_message(const std::string& login, const std::string& message) {
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
    net::post(io_context_, [this, login, message]() {{
            std::optional<std::string> destination_address = dht_ip_resolver_.resolve(login);
            if (destination_address) {
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is set" << std::endl;
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Sending message to " << login << '\n';
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Calling send_message" << '\n';
                net::post(io_context_, [this, login, message]() {
                    message_sender_.send_message(login, message);
                });
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Called send_message" << '\n';
            } else {
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is not set" << std::endl;
            }
        }});
}

std::optional<std::string> P2PMessengerImpl::resolve(const std::string& login) {
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "on_search_login called" << std::endl;
    if (login.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Login is empty" << std::endl;
        return std::nullopt;
    }
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Searching for login: " << login << std::endl;
    std::optional<std::string> address = dht_ip_resolver_.resolve(login);
    if (address) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Found address: " << *address << std::endl;
    } else {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Address not found" << std::endl;
    }
    return address;
}

void P2PMessengerImpl::listen(const std::string& login) {
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "on_listen called" << std::endl;
    if (login.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Login is empty" << std::endl;
        return;
    }
    dht_ip_resolver_.listen(login);
}
