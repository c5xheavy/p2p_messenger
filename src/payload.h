#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <cstdint>
#include <string>

struct Payload {
    uint64_t time;
    std::string text;
};

#endif // PAYLOAD_H
