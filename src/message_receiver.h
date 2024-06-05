#ifndef MESSAGE_RECEIVER_H
#define MESSAGE_RECEIVER_H

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/system.hpp>
#include <opendht.h>

#include "message.h"

namespace net = boost::asio;
using net::ip::udp;
namespace sys = boost::system;

class MessageReceiver {
public:
    using ReceiveMessageHandler = std::function<void(Message&&)>;

    MessageReceiver(udp::socket& socket, std::shared_ptr<dht::crypto::PrivateKey> private_key, ReceiveMessageHandler handler);
    ~MessageReceiver();

private:
    void async_wait();
    void async_wait_handler(const sys::error_code& ec);

private:
    udp::socket& socket_;
    std::shared_ptr<dht::crypto::PrivateKey> private_key_;
    ReceiveMessageHandler handler_;
};

#endif // MESSAGE_RECEIVER_H
