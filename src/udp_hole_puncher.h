#ifndef UDP_HOLE_PUNCHER_H
#define UDP_HOLE_PUNCHER_H

#include <cstdint>
#include <chrono>
#include <memory>
#include <iostream>
#include <string>
#include <vector>

#include <boost/asio.hpp>

namespace net = boost::asio;
using net::ip::udp;
namespace sys = boost::system;

class UdpHolePuncher {
public:
    UdpHolePuncher(udp::socket& socket);

    void start_hole_punching(const std::string& ip, uint16_t port, net::system_timer::duration interval = std::chrono::seconds{10});

private:
    void hole_punch(std::shared_ptr<udp::endpoint> endpoint, net::system_timer::duration interval);

private:
    udp::socket& socket_;
    net::system_timer timer_;
    std::vector<uint8_t> buffer{1};
};

#endif // UDP_HOLE_PUNCHER_H
