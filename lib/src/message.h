#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <string>
#include <vector>

#include "payload.h"

using Buffer = std::vector<std::uint8_t>;

struct Message {
    std::uint64_t id;
    std::string sender_login;
    std::string recipient_login;
    Payload payload;
};

#endif
