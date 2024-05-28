#ifndef MESSAGE_SERIALIZER_H
#define MESSAGE_SERIALIZER_H

#include <cstdint>
#include <memory>
#include <vector>

#include <opendht.h>

#include "message.h"

class MessageSerializer {
public:
    static std::vector<uint8_t> message_to_buffer(const Message& message, std::shared_ptr<dht::crypto::PublicKey> public_key = nullptr);
};

#endif // MESSAGE_SERIALIZER_H
