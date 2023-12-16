#ifndef DATAGRAM_DESERIALIZER_H
#define DATAGRAM_DESERIALIZER_H

#include <string>
#include <vector>

#include "message.h"

class MessageDeserializer {
public:
    static Message MessageFromBuffer(const Buffer& buffer);
};

#endif
