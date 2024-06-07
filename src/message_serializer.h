#ifndef MESSAGE_SERIALIZER_H
#define MESSAGE_SERIALIZER_H

#include <cstdint>
#include <memory>
#include <vector>

#include <opendht.h>

#include "message.h"
#include "signed_message.h"

class MessageSerializer {
public:
    static std::vector<uint8_t> message_to_buffer(const Message& message, std::shared_ptr<dht::crypto::PublicKey> public_key);
    static std::vector<uint8_t> sign(const std::vector<uint8_t>& buffer, std::shared_ptr<dht::crypto::PrivateKey> private_key);
    static std::vector<uint8_t> signed_message_to_buffer(const SignedMessage& signed_message);
    static std::vector<uint8_t> metadata_to_buffer(const std::string& login, const std::string public_key);
};

#endif // MESSAGE_SERIALIZER_H
