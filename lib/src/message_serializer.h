#ifndef MESSAGE_SERIALIZER_H
#define MESSAGE_SERIALIZER_H

#include <cstddef>
#include <memory>
#include <utility>

#include "message.h"

class MessageSerializer {
public:
    static std::pair<std::shared_ptr<char[]>, std::size_t> MessageToBuffer(const Message& message);
};

#endif // MESSAGE_SERIALIZER_H
