#ifndef MESSAGE_SERIALIZER_H
#define MESSAGE_SERIALIZER_H

#include <cstdint>
#include <vector>

#include "message.h"

class MessageSerializer {
public:
    static std::vector<uint8_t> message_to_buffer(const Message& message);
};

#endif // MESSAGE_SERIALIZER_H
