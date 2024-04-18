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

#include <opendht.h>

class DhtIpResolver{
public:
    DhtIpResolver(std::uint16_t port);

    ~DhtIpResolver();

    void put(const std::string& login, const std::string& ip, std::uint16_t port);

    void listen(const std::string& login);

    std::optional<std::string> Resolve(const std::string& login);

private:
    dht::DhtRunner node;

    std::map<std::string, std::string> login_to_address;
    std::mutex login_to_address_mutex;

    std::map<std::string, std::future<std::size_t>> login_to_token;
    std::mutex login_to_token_mutex;
};

#endif // DHT_IP_RESOLVER_H
