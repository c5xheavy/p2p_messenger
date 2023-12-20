#include "login_hasher.h"

std::string LoginHasher::Hash(const std::string& login) {
    if (login.size() > hash_size) {
        return login.substr(0, hash_size);
    }
    return login + std::string(hash_size - login.size(), '#');
}
