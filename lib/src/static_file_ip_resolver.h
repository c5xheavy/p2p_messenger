#ifndef STATIC_FILE_IP_RESOLVER_H
#define STATIC_FILE_IP_RESOLVER_H

#include <filesystem>
#include <map>
#include <optional>
#include <string>

class StaticFileIpResolver {
public:
    StaticFileIpResolver(const std::filesystem::path& static_file_path);
    
    std::optional<std::string> Resolve(const std::string& login);
private:
    std::map<std::string, std::string> login_to_ip_;
};

#endif // STATIC_FILE_IP_RESOLVER_H
