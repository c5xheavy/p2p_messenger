#ifndef DHT_IP_RESOLVER_H
#define DHT_IP_RESOLVER_H

#include <optional>
#include <string>

class DhtIpResolver{
public:
    std::optional<std::string> Resolve(const std::string& login);
};

#endif // DHT_IP_RESOLVER_H
