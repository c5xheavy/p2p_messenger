#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <string>
#include <vector>

#include "message.h"

using Buffer = std::vector<std::uint8_t>;

struct Datagram {
    std::uint64_t id;
    std::string sender_login;
    std::string recipient_login;
    Message message;
};

#endif
