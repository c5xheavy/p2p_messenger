#ifndef METADATA_IP_RESOLVER_H
#define METADATA_IP_RESOLVER_H

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include <opendht.h>

class MetadataIpResolver {
public:
    void put(const std::string& login, std::shared_ptr<dht::crypto::PublicKey> public_key, const std::string& ip, uint16_t port);
    std::optional<std::string> resolve(const std::string& login, const dht::InfoHash& public_key_id);
    std::shared_ptr<dht::crypto::PublicKey> get_public_key_by_public_key_id(const dht::InfoHash& public_key_id);

private:
    std::map<std::string, std::map<dht::InfoHash, std::string>> login_to_public_key_id_to_address_;
    std::mutex login_to_public_key_id_to_address_mutex_;
    std::map<dht::InfoHash, std::shared_ptr<dht::crypto::PublicKey>> public_key_id_to_public_key_;
    std::mutex public_key_id_to_public_key_mutex_;
};

#endif // METADATA_IP_RESOLVER_H
