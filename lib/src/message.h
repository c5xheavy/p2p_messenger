#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <string>
#include <vector>

#include "payload.h"

struct Message {
    std::uint64_t id;
    std::string source_login;
    std::string destination_login;
    Payload payload;
};

#endif
