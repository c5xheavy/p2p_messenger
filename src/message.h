#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <string>

#include "payload.h"

struct Message {
    uint64_t id;
    std::string source_ip;
    uint16_t source_port;
    std::string source_login;
    std::string source_public_key;
    std::string destination_ip;
    uint16_t destination_port;
    std::string destination_login;
    std::string destination_public_key;
    Payload payload;
};

#endif // MESSAGE_H
