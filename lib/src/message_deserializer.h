#ifndef MESSAGE_DESERIALIZER_H
#define MESSAGE_DESERIALIZER_H

#include <string>

#include "message.h"

class MessageDeserializer {
public:
    static Message MessageFromBuffer(char* buffer, std::size_t buffer_size);
};

#endif // MESSAGE_DESERIALIZER_H
