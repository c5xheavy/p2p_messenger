#include "dht_ip_resolver.h"

#include <cstdint>
#include <cstddef>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <syncstream>
#include <thread>
#include <vector>

#include <boost/asio.hpp>
#include <boost/system.hpp>
#include <opendht.h>

namespace net = boost::asio;
namespace sys = boost::system;

DhtIpResolver::DhtIpResolver(net::io_context& io_context, uint16_t port,
                             const std::string& bootstrap_node_ip, uint16_t bootstrap_node_port,
                             const dht::crypto::Identity& identity, ListenLoginHandler handler)
    : node_{}
    , port_{port}
    , identity_{identity}
    , timer_{io_context}
    , login_to_public_key_id_to_address_{}
    , login_to_public_key_id_to_address_mutex_{}
    , public_key_id_to_public_key_{}
    , public_key_id_to_public_key_mutex_{}
    , login_to_token_{}
    , login_to_token_mutex_{}
    , handler_{handler} {
    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port.
    node_.run(port_, identity_, true);

    // Join the network through any running node,
    // here using a known bootstrap node.
    node_.bootstrap(bootstrap_node_ip, std::to_string(bootstrap_node_port));
}

DhtIpResolver::~DhtIpResolver() {
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "DhtIpResolver destructor called" << std::endl;
    {
        std::lock_guard<std::mutex> lock{login_to_token_mutex_};
        for (auto& [login, token] : login_to_token_) {
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Cancelling listen for " << login << std::endl;
            auto key = dht::InfoHash::get(login);
            node_.cancelListen(key, std::move(token));
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Cancelled listen" << std::endl;
        }
    }
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Joining node" << std::endl;
    node_.join();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Joined node" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "DhtIpResolver destructor finished" << std::endl;
}

void DhtIpResolver::put(const std::string& login, const std::string& ip, uint16_t port, net::system_timer::duration interval) {
    std::string address{ip + ":" + std::to_string(port)};
    put(std::make_shared<std::string>(login), std::make_shared<std::string>(address), interval);
}

void DhtIpResolver::put_signed(const std::string& login, const std::string& ip, uint16_t port, net::system_timer::duration interval) {
    std::string address{ip + ":" + std::to_string(port)};
    put_signed(std::make_shared<std::string>(login), std::make_shared<std::string>(address), interval);
}

void DhtIpResolver::put(std::shared_ptr<std::string> login, std::shared_ptr<std::string> address, net::system_timer::duration interval) {
    node_.put(dht::InfoHash::get(*login), {(const uint8_t*)address->data(), address->size()});
    timer_.expires_after(interval);
    timer_.async_wait([this, login, address, interval](const sys::error_code& ec) {
        if (ec) {
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Timer error: " << ec.message() << std::endl;
        }
        put(login, address, interval);
    });
}

void DhtIpResolver::put_signed(std::shared_ptr<std::string> login, std::shared_ptr<std::string> address, net::system_timer::duration interval) {
    node_.putSigned(dht::InfoHash::get(*login), {(const uint8_t*)address->data(), address->size()});
    timer_.expires_after(interval);
    timer_.async_wait([this, login, address, interval](const sys::error_code& ec) {
        if (ec) {
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Timer error: " << ec.message() << std::endl;
        }
        put_signed(login, address, interval);
    });
}

void DhtIpResolver::listen(const std::string& login) {
    {
        std::lock_guard<std::mutex> lock{login_to_token_mutex_};
        if (login_to_token_.find(login) != login_to_token_.end()) {
            return;
        }
    }
    auto key = dht::InfoHash::get(login);
    std::future<size_t> token = node_.listen(key,
        [this, login](const std::vector<std::shared_ptr<dht::Value>>& values, bool expired) {
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "listen callback called for login " << login << std::endl;
            for (const auto& value : values) {
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Found value: " << *value << ", " << (expired ? "expired" : "added") << std::endl;
                std::string address{value->data.begin(), value->data.end()};
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Found address: " << address << std::endl;
                if (!expired) {
                    size_t pos = address.find(':');
                    if (pos == std::string::npos) {
                        throw std::invalid_argument{"Invalid address"};
                    }
                    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Update destination address to " << address << std::endl;
                    if (value->owner) {
                        dht::InfoHash public_key_id{value->owner->getId()};
                        {
                            std::lock_guard<std::mutex> lock{login_to_public_key_id_to_address_mutex_};
                            login_to_public_key_id_to_address_[login][public_key_id] = address;
                        }
                        {
                            std::lock_guard<std::mutex> lock{public_key_id_to_public_key_mutex_};
                            public_key_id_to_public_key_[public_key_id] =  value->owner;
                        }
                        handler_({login, std::move(public_key_id)});
                    } else {
                        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "In listen public key is null" << std::endl;
                    }
                }
            }
            return true; // keep listening
        }
    );
    {
        std::lock_guard<std::mutex> lock{login_to_token_mutex_};
        login_to_token_[login] = std::move(token);
    }
}

std::optional<std::string> DhtIpResolver::resolve(const std::string& login, const dht::InfoHash& public_key_id) {
    std::lock_guard<std::mutex> lock{login_to_public_key_id_to_address_mutex_};
    auto login_map_it = login_to_public_key_id_to_address_.find(login);
    if (login_map_it == login_to_public_key_id_to_address_.end()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Login is not listened for" << std::endl;
        return std::nullopt;
    }
    auto public_key_id_it = login_map_it->second.find(public_key_id);
    if (public_key_id_it == login_map_it->second.end()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Public key is not found" << std::endl;
        return std::nullopt;
    }
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is set" << std::endl;
    return public_key_id_it->second;
}

std::shared_ptr<dht::crypto::PublicKey> DhtIpResolver::get_public_key_by_public_key_id(const dht::InfoHash& public_key_id) {
    std::lock_guard<std::mutex> lock{public_key_id_to_public_key_mutex_};
    auto it = public_key_id_to_public_key_.find(public_key_id);
    if (it == public_key_id_to_public_key_.end()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "No public key with such public key ID" << std::endl;
        return nullptr;
    }
    return it->second;
}
