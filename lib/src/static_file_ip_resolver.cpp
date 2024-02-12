#include "static_file_ip_resolver.h"

#include <fstream>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>

#include <iostream>

#include "login_hasher.h"

StaticFileIpResolver::StaticFileIpResolver(const std::filesystem::path& static_file_path) {
    std::ifstream ifs{static_file_path.string()};
    if (!ifs.good()) {
        throw std::runtime_error("Config file doesn't exists");
    }
    while (!ifs.eof()) {
        std::string login, ip;
        ifs >> login >> ip;
        login_hash_to_ip_[LoginHasher::Hash(login)] = ip;
    }
}

std::optional<std::string> StaticFileIpResolver::Resolve(const std::string& login_hash) {
    auto it = login_hash_to_ip_.find(login_hash);
    if (it == login_hash_to_ip_.end()) {
        return std::nullopt;
    }
    return it->second;
}
