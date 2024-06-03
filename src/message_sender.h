#ifndef MESSAGE_SENDER_H
#define MESSAGE_SENDER_H

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include <boost/asio.hpp>

#include "dht_ip_resolver.h"
#include "message.h"

namespace net = boost::asio;
using net::ip::udp;

class MessageSender {
public:
    using SendMessageHandler = std::function<void(Message&& message)>;

    MessageSender(net::io_context& io_context, DhtIpResolver& dht_ip_resolver, const std::string& source_ip, uint16_t source_port,
                  const std::string& source_login, const dht::crypto::Identity& identity, SendMessageHandler handler);
    ~MessageSender();

    void send_message(const std::string& destination_address, const std::string& destination_login,
                      std::shared_ptr<dht::crypto::PublicKey> public_key, const std::string& text);

private:
    std::pair<std::string, uint16_t> get_ip_and_port_from_address(const std::string& address);

private:
    net::io_context& io_context_;
    DhtIpResolver& dht_ip_resolver_;
    std::string source_ip_;
    uint16_t source_port_;
    std::string source_login_;
    dht::crypto::Identity identity_;
    udp::socket socket_;
    SendMessageHandler handler_;
};

#endif // MESSAGE_SENDER_H
