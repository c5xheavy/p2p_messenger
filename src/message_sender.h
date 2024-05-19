#ifndef MESSAGE_SENDER_H
#define MESSAGE_SENDER_H

#include <cstdint>
#include <string>
#include <utility>

#include <boost/asio.hpp>

#include "dht_ip_resolver.h"

namespace net = boost::asio;
using net::ip::udp;

class MessageSender {
public:
    using SendMessageHandler = std::function<void(const std::string&, const std::string&)>;

    MessageSender(net::io_context& io_context, DhtIpResolver& dht_ip_resolver, const std::string& source_login, SendMessageHandler handler);
    ~MessageSender();

    void send_message(const std::string& destination_login, const std::string& text);

private:
    std::pair<std::string, std::uint16_t> get_ip_and_port_from_address(const std::string& address);

private:
    net::io_context& io_context_;
    DhtIpResolver& dht_ip_resolver_;
    std::string source_login_;
    udp::socket socket_;
    SendMessageHandler handler_;
};

#endif // MESSAGE_SENDER_H
