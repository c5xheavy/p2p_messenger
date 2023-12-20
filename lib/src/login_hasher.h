#ifndef LOGIN_HASHER_H
#define LOGIN_HASHER_H

#include <string>

class LoginHasher {
public:
    static std::string hash(const std::string& login);

    static constexpr std::size_t hash_size = 512;
};

#endif
