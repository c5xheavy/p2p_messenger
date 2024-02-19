#ifndef LOGIN_HASHER_H
#define LOGIN_HASHER_H

#include <cstddef>
#include <string>

class LoginHasher {
public:
    static std::string Hash(const std::string& login);

    static constexpr std::size_t hash_size{64};
};

#endif // LOGIN_HASHER_H
