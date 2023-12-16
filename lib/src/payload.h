#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <string>

struct Payload {
    std::string sender_ip;
    int64_t time;
    std::string text;
};

#endif
