#include "udp_hole_puncher.h"

#include "message_serializer.h"

UdpHolePuncher::UdpHolePuncher(udp::socket& socket, net::io_context& io_context, const std::string& login, const std::string public_key)
    : socket_{socket} 
    , io_context_{io_context}
    , login_{login}
    , public_key_{public_key}
    , buffer_{MessageSerializer::metadata_to_buffer(login_, public_key_)} {
}

void UdpHolePuncher::start_hole_punching(const std::string& ip, uint16_t port, net::system_timer::duration interval) {
    hole_punch(std::make_shared<net::system_timer>(io_context_), std::make_shared<udp::endpoint>(net::ip::make_address(ip), port), interval);
}

void UdpHolePuncher::hole_punch(std::shared_ptr<net::system_timer> timer, std::shared_ptr<udp::endpoint> endpoint, net::system_timer::duration interval) {
    socket_.send_to(net::buffer(buffer_), *endpoint);
    timer->expires_after(interval);
    timer->async_wait([this, timer, endpoint, interval](const sys::error_code& ec) {
        if (ec) {
            std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Timer error: " << ec.message() << std::endl;
        }
        hole_punch(timer, endpoint, interval);
    });
}