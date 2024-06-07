#include "udp_hole_puncher.h"

UdpHolePuncher::UdpHolePuncher(udp::socket& socket)
    : socket_{socket} 
    , timer_{socket_.get_executor()} {
}

void UdpHolePuncher::start_hole_punching(const std::string& ip, uint16_t port, net::system_timer::duration interval) {
    hole_punch(std::make_shared<udp::endpoint>(net::ip::make_address(ip), port), interval);
}

void UdpHolePuncher::hole_punch(std::shared_ptr<udp::endpoint> endpoint, net::system_timer::duration interval) {
    socket_.send_to(net::buffer(buffer), *endpoint);
    timer_.expires_after(interval);
    timer_.async_wait([this, endpoint, interval](const sys::error_code& ec) {
        if (ec) {
            std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Timer error: " << ec.message() << std::endl;
        }
        hole_punch(endpoint, interval);
    });
}