#ifndef IP_RESOLVER_H
#define IP_RESOLVER_H

#include <optional>
#include <string>

class IpResolver {
public:
    virtual std::optional<std::string> Resolve(const std::string& login_hash) = 0;
    virtual ~IpResolver() = default;
};

#endif // IP_RESOLVER_H
