#ifndef MESSAGE_DESERIALIZER_H
#define MESSAGE_DESERIALIZER_H

#include <cstddef>

#include "message.h"

class MessageDeserializer {
public:
    static Message message_from_buffer(char* buffer, std::size_t buffer_size);
};

#endif // MESSAGE_DESERIALIZER_H
