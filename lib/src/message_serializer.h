#ifndef DATAGRAM_SERIALIZER_H
#define DATAGRAM_SERIALIZER_H

#include <memory>
#include <string>
#include <vector>

#include "message.h"

class MessageSerializer {
public:
    static std::pair<std::shared_ptr<char[]>, std::size_t> MessageToBuffer(const Message& message);
};

#endif
