#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <string>

#include "payload.h"

struct Message {
    std::uint64_t id;
    std::string source_login;
    std::string destination_login;
    Payload payload;
    static const std::size_t login_size{64}; // temporary solution
};

#endif // MESSAGE_H
