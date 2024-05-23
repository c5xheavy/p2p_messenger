#ifndef P2P_MESSENGER_IMPL_H
#define P2P_MESSENGER_IMPL_H

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <boost/asio.hpp>
#include <opendht.h>

#include "dht_ip_resolver.h"
#include "message.h"
#include "message_receiver.h"
#include "message_sender.h"
#include "message_serializer.h"
#include "message_deserializer.h"

namespace net = boost::asio;
using net::ip::udp;
namespace sys = boost::system;

class P2PMessengerImpl
{
public:
    P2PMessengerImpl(const std::string& my_login, std::uint16_t dht_port,
                     const std::string& my_ip, std::uint16_t my_port,
                     bool generate_crypto_identity, const std::string& crypto_identity_path,
                     MessageSender::SendMessageHandler send_message_handler,
                     MessageReceiver::ReceiveMessageHandler receive_message_handler,
                     DhtIpResolver::ListenLoginHandler listen_login_handler);
    ~P2PMessengerImpl();

public:
    void send_message(const std::string& login, const std::string& message);

    std::optional<std::string> resolve(const std::string& login);

    void listen(const std::string& login);

private:
    dht::crypto::Identity get_identity(bool generate_crypto_identity, const std::string& crypto_identity_path);

private:
    std::uint16_t dht_port_; 
    std::string my_ip_;
    std::uint16_t my_port_; 
    std::string my_login_;
    bool generate_crypto_identity_;
    const std::string& crypto_identity_path_;

    const std::size_t num_threads_;

    net::io_context io_context_;

    net::executor_work_guard<net::io_context::executor_type> work_guard_;

    DhtIpResolver dht_ip_resolver_;

    std::vector<std::jthread> threads_;

    MessageSender message_sender_;

    MessageReceiver message_receiver_;
};

#endif // P2P_MESSENGER_IMPL_H
