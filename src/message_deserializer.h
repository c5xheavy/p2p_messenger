#ifndef MESSAGE_DESERIALIZER_H
#define MESSAGE_DESERIALIZER_H

#include <cstdint>
#include <vector>

#include "message.h"

class MessageDeserializer {
public:
    static Message message_from_buffer(const std::vector<uint8_t>& buffer);
};

#endif // MESSAGE_DESERIALIZER_H
