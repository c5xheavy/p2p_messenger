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
    using ReceiveMessageHandler = std::function<void(const std::string&, const std::string&)>;
public:
    MessageReceiver(net::io_context& io_context, std::uint16_t port);
    ~MessageReceiver();

    void start_async_receive(ReceiveMessageHandler&& handler);

private:
    void async_wait(ReceiveMessageHandler&& handler);
    void async_wait_handler(ReceiveMessageHandler&& handler, const sys::error_code& ec);

private:
    net::io_context& io_context_;
    udp::socket socket_;
};

#endif // MESSAGE_RECEIVER_H
