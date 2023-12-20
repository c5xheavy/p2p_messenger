#include "login_hasher.h"

std::string LoginHasher::hash(const std::string& login) {
    return login + std::string(hash_size - login.size(), '#');
}
