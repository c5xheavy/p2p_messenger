#ifndef DHT_IP_RESOLVER_H
#define DHT_IP_RESOLVER_H

#include <cstdint>
#include <cstddef>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <opendht.h>

namespace net = boost::asio;

class DhtIpResolver{
public:
    using ListenLoginHandler = std::function<void(const std::string&, const std::string&)>;
public:
    DhtIpResolver(net::io_context& io_context, std::uint16_t port, ListenLoginHandler handler);

    ~DhtIpResolver();

    void put(const std::string& login, const std::string& ip, std::uint16_t port, net::system_timer::duration interval = std::chrono::seconds{60});

    void listen(const std::string& login);

    std::optional<std::string> Resolve(const std::string& login);

private:
    void put(std::shared_ptr<std::string> login, std::shared_ptr<std::string> address, net::system_timer::duration interval);

private:
    net::io_context& io_context_;
    net::system_timer timer_;

    dht::DhtRunner node_;

    std::map<std::string, std::string> login_to_address_;
    std::mutex login_to_address_mutex_;

    std::map<std::string, std::future<std::size_t>> login_to_token_;
    std::mutex login_to_token_mutex_;

    ListenLoginHandler handler_;
};

#endif // DHT_IP_RESOLVER_H
