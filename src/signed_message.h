#ifndef SIGNED_MESSAGE_H
#define SIGNED_MESSAGE_H

#include <cstdint>
#include <vector>

#include "message.h"

struct SignedMessage {
    std::vector<uint8_t> message;
    std::vector<uint8_t> signature;
};

#endif // SIGNED_MESSAGE_H
