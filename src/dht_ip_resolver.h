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

#include "contact.h"

namespace net = boost::asio;

class DhtIpResolver {
public:
    using ListenLoginHandler = std::function<void(Contact&&)>;

    DhtIpResolver(net::io_context& io_context, uint16_t port, const dht::crypto::Identity& identity, ListenLoginHandler handler);
    ~DhtIpResolver();

    void put(const std::string& login, const std::string& ip, uint16_t port, net::system_timer::duration interval = std::chrono::seconds{60});
    void put_signed(const std::string& login, const std::string& ip, uint16_t port, net::system_timer::duration interval = std::chrono::seconds{60});
    void listen(const std::string& login);
    std::optional<std::string> resolve(const std::string& login, const dht::InfoHash& public_key_id);
    std::shared_ptr<dht::crypto::PublicKey> get_public_key_by_public_key_id(const dht::InfoHash& public_key_id);

private:
    void put(std::shared_ptr<std::string> login, std::shared_ptr<std::string> address, net::system_timer::duration interval);
    void put_signed(std::shared_ptr<std::string> login, std::shared_ptr<std::string> address, net::system_timer::duration interval);

private:
    dht::DhtRunner node_;
    uint16_t port_;
    dht::crypto::Identity identity_;
    net::system_timer timer_;
    std::map<std::string, std::map<dht::InfoHash, std::string>> login_to_public_key_id_to_address_;
    std::mutex login_to_public_key_id_to_address_mutex_;
    std::map<dht::InfoHash, std::shared_ptr<dht::crypto::PublicKey>> public_key_id_to_public_key_;
    std::mutex public_key_id_to_public_key_mutex_;
    std::map<std::string, std::future<size_t>> login_to_token_;
    std::mutex login_to_token_mutex_;
    ListenLoginHandler handler_;
};

#endif // DHT_IP_RESOLVER_H
