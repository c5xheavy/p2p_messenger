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

DhtIpResolver::DhtIpResolver(net::io_context& io_context, std::uint16_t port, ListenLoginHandler handler)
    : io_context_{io_context}
    , timer_{io_context}
    , node_{}
    , login_to_address_{}
    , login_to_address_mutex_{}
    , login_to_token_{}
    , login_to_token_mutex_{}
    , handler_{handler} {
    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port.
    node_.run(port, dht::crypto::generateIdentity(), true);

    // Join the network through any running node,
    // here using a known bootstrap node.
    node_.bootstrap("bootstrap.jami.net", "4222");
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

void DhtIpResolver::Put(const std::string& login, const std::string& ip, std::uint16_t port, net::system_timer::duration interval) {
    std::string address{ip + ":" + std::to_string(port)};
    put(std::make_shared<std::string>(login), std::make_shared<std::string>(address), interval);
}

void DhtIpResolver::put(std::shared_ptr<std::string> login, std::shared_ptr<std::string> address, net::system_timer::duration interval) {
    node_.put(dht::InfoHash::get(*login), {(const std::uint8_t*)address->data(), address->size()});
    timer_.expires_after(interval);
    timer_.async_wait([this, login, address, interval](const sys::error_code& ec) {
        if (ec) {
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Timer error: " << ec.message() << std::endl;
        }
        put(login, address, interval);
    });
}

void DhtIpResolver::Listen(const std::string& login) {
    {
        std::lock_guard<std::mutex> lock{login_to_address_mutex_};
        if (login_to_address_.find(login) != login_to_address_.end()) {
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
                    std::size_t pos = address.find(':');
                    if (pos == std::string::npos) {
                        throw std::invalid_argument{"Invalid address"};
                    }
                    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Update destination address to " << address << std::endl;
                    {
                        std::lock_guard<std::mutex> lock{login_to_address_mutex_};
                        login_to_address_[login] = address;
                    }
                    handler_(login, address);
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

std::optional<std::string> DhtIpResolver::Resolve(const std::string& login) {
    std::lock_guard<std::mutex> lock{login_to_address_mutex_};
    auto it = login_to_address_.find(login);
    if (it == login_to_address_.end()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is not set" << std::endl;
        return std::nullopt;
    }
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is set" << std::endl;
    return it->second;
}
