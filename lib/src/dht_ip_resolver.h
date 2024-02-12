#ifndef DHT_IP_RESOLVER_H
#define DHT_IP_RESOLVER_H

#include "ip_resolver.h"

#include <optional>
#include <string>

class DhtIpResolver : public IpResolver {
public:
    std::optional<std::string> Resolve(const std::string& login_hash) override;
};

#endif // DHT_IP_RESOLVER_H
