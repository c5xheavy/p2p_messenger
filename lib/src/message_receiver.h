#ifndef MESSAGE_RECEIVER_H
#define MESSAGE_RECEIVER_H

#include <cstdint>
#include <functional>
#include <string>

#include <boost/asio.hpp>
#include <boost/system.hpp>

namespace net = boost::asio;
using net::ip::udp;
namespace sys = boost::system;

class MessageReceiver {
public:
    using ReceiveMessageHandler = std::function<void(const std::string&, const std::string&)>;

    MessageReceiver(net::io_context& io_context, std::uint16_t port, ReceiveMessageHandler handler);
    ~MessageReceiver();

private:
    void async_wait();
    void async_wait_handler(const sys::error_code& ec);

private:
    net::io_context& io_context_;
    udp::socket socket_;
    ReceiveMessageHandler handler_;
};

#endif // MESSAGE_RECEIVER_H
