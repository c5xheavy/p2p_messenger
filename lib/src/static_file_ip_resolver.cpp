#include "static_file_ip_resolver.h"

#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>

StaticFileIpResolver::StaticFileIpResolver(const std::filesystem::path& static_file_path) {
    std::ifstream ifs{static_file_path.string()};
    if (!ifs.good()) {
        throw std::runtime_error{"Config file doesn't exists"};
    }
    while (!ifs.eof()) {
        std::string login, ip;
        ifs >> login >> ip;
        login_to_ip_.emplace(login, ip);
    }
}

std::optional<std::string> StaticFileIpResolver::Resolve(const std::string& login) {
    auto it{login_to_ip_.find(login)};
    if (it == login_to_ip_.end()) {
        return std::nullopt;
    }
    return it->second;
}
