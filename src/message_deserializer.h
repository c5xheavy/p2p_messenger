#ifndef MESSAGE_DESERIALIZER_H
#define MESSAGE_DESERIALIZER_H

#include <cstdint>
#include <memory>
#include <vector>

#include <opendht.h>

#include "message.h"

class MessageDeserializer {
public:
    static Message message_from_buffer(const std::vector<uint8_t>& buffer, std::shared_ptr<dht::crypto::PrivateKey> private_key = nullptr);
};

#endif // MESSAGE_DESERIALIZER_H
