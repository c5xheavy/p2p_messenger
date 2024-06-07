#ifndef MESSAGE_RECEIVER_H
#define MESSAGE_RECEIVER_H

#include <cstdint>
#include <fstream>
#include <functional>
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/system.hpp>
#include <opendht.h>

#include "message.h"
#include "metadata_ip_resolver.h"
#include "udp_hole_puncher.h"

namespace net = boost::asio;
using net::ip::udp;
namespace sys = boost::system;

class MessageReceiver {
public:
    using ReceiveMessageHandler = std::function<void(Message&&)>;

    MessageReceiver(udp::socket& socket, std::shared_ptr<dht::crypto::PrivateKey> private_key, MetadataIpResolver& metadata_ip_resolver,
                    bool relay, UdpHolePuncher& upd_hole_puncher, ReceiveMessageHandler handler);
    ~MessageReceiver();

private:
    void async_wait();
    void async_wait_handler(const sys::error_code& ec);

private:
    udp::socket& socket_;
    std::shared_ptr<dht::crypto::PrivateKey> private_key_;
    MetadataIpResolver& metadata_ip_resolver_;
    bool relay_;
    UdpHolePuncher& upd_hole_puncher_;
    ReceiveMessageHandler handler_;
    std::fstream ofs{"log.txt", std::ios_base::app};
};

#endif // MESSAGE_RECEIVER_H
