#ifndef MESSAGE_DESERIALIZER_H
#define MESSAGE_DESERIALIZER_H

#include <cstdint>
#include <memory>
#include <vector>

#include <opendht.h>

#include "message.h"
#include "signed_message.h"

class MessageDeserializer {
public:
    static SignedMessage signed_message_from_buffer(const std::vector<uint8_t>& buffer);
    static bool check_signature(const SignedMessage& signed_message, std::shared_ptr<dht::crypto::PublicKey> public_key);
    static Message message_from_buffer(const std::vector<uint8_t>& buffer, std::shared_ptr<dht::crypto::PrivateKey> private_key);
    static Message message_with_not_decrypted_payload_from_buffer(const std::vector<uint8_t>& buffer);
    static std::string destinantion_public_key_from_buffer(const std::vector<uint8_t>& buffer);
    static std::pair<std::string, std::string> metadata_from_buffer(const std::vector<uint8_t>& buffer);
};

#endif // MESSAGE_DESERIALIZER_H
