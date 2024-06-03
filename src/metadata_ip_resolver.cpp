#include "metadata_ip_resolver.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <syncstream>
#include <thread>

#include <opendht.h>

void MetadataIpResolver::put(const std::string& login, std::shared_ptr<dht::crypto::PublicKey> public_key, const std::string& ip, uint16_t port) {
    std::string address{ip + ":" + std::to_string(port)};
    {
        std::lock_guard<std::mutex> lock{login_to_public_key_id_to_address_mutex_};
        login_to_public_key_id_to_address_[login][public_key->getId()] = address;
    }
    {
        std::lock_guard<std::mutex> lock{public_key_id_to_public_key_mutex_};
        public_key_id_to_public_key_[public_key->getId()] = public_key;
    }
}

std::optional<std::string> MetadataIpResolver::resolve(const std::string& login, const dht::InfoHash& public_key_id) {
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

std::shared_ptr<dht::crypto::PublicKey> MetadataIpResolver::get_public_key_by_public_key_id(const dht::InfoHash& public_key_id) {
    std::lock_guard<std::mutex> lock{public_key_id_to_public_key_mutex_};
    auto it = public_key_id_to_public_key_.find(public_key_id);
    if (it == public_key_id_to_public_key_.end()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "No public key with such public key ID" << std::endl;
        return nullptr;
    }
    return it->second;
}
