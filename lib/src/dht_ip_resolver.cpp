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

#include <opendht.h>

DhtIpResolver::DhtIpResolver(std::uint16_t port) {
    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port.
    node.run(port, dht::crypto::generateIdentity(), true);

    // Join the network through any running node,
    // here using a known bootstrap node.
    node.bootstrap("bootstrap.jami.net", "4222");
}

DhtIpResolver::~DhtIpResolver() {
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "DhtIpResolver destructor called" << std::endl;
    {
        std::lock_guard<std::mutex> lock{login_to_token_mutex};
        for (auto& [login, token] : login_to_token) {
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Cancelling listen for " << login << std::endl;
            auto key = dht::InfoHash::get(login);
            node.cancelListen(key, std::move(token));
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Cancelled listen" << std::endl;
        }
    }
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Joining node" << std::endl;
    node.join();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Joined node" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "DhtIpResolver destructor finished" << std::endl;
}

void DhtIpResolver::put(const std::string& login, const std::string& ip, std::uint16_t port) {
    std::string address{ip + ":" + std::to_string(port)};
    node.put(dht::InfoHash::get(login), {(const std::uint8_t*)address.data(), address.size()});
}

void DhtIpResolver::listen(const std::string& login) {
    auto key = dht::InfoHash::get(login);
    std::future<size_t> token = node.listen(key,
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
                        std::lock_guard<std::mutex> lock{login_to_address_mutex};
                        login_to_address[login] = address;
                    }
                }
            }
            return true; // keep listening
        }
    );
    {
        std::lock_guard<std::mutex> lock{login_to_token_mutex};
        login_to_token[login] = std::move(token);
    }
}

std::optional<std::string> DhtIpResolver::Resolve(const std::string& login) {
    std::lock_guard<std::mutex> lock{login_to_address_mutex};
    auto it = login_to_address.find(login);
    if (it == login_to_address.end()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is not set" << std::endl;
        return std::nullopt;
    }
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is set" << std::endl;
    return it->second;
}
