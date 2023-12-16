#ifndef DATAGRAM_SERIALIZER_H
#define DATAGRAM_SERIALIZER_H

#include <string>
#include <vector>

#include "message.h"

class MessageSerializer {
public:
    static Buffer MessageToBuffer(const Message& message);
};

#endif
