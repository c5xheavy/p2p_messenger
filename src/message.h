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
};

#endif // MESSAGE_H
