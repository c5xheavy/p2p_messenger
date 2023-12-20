#ifndef STATIC_FILE_IP_RESOLVER_H
#define STATIC_FILE_IP_RESOLVER_H

#include "ip_resolver.h"

#include <filesystem>
#include <map>
#include <optional>
#include <string>

class StaticFileIpResolver : public IpResolver {
public:
    StaticFileIpResolver(const std::filesystem::path& static_file_path);
    
    std::optional<std::string> Resolve(const std::string& login_hash) override;
private:
    std::map<std::string, std::string> login_hash_to_ip_;
};

#endif
