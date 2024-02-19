#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <cstdint>
#include <string>

struct Payload {
    std::uint64_t time;
    std::string text;
};

#endif // PAYLOAD_H
