#include "message_serializer.h"
#include "payload_serializer.h"

#include <algorithm>

Buffer MessageSerializer::MessageToBuffer(const Message& message) {
    Buffer buffer;
    for (int i = 0; i < 8; ++i) {
        // is it UB?
        // buffer.push_back(static_cast<std::uint8_t>(message.id >> (i * 8)));
        buffer.push_back((message.id >> (i * 8)) & 0xff);
    }
    // login size <= 64 = 1 byte
    buffer.push_back(message.sender_login.size());
    std::copy(message.sender_login.begin(), message.sender_login.end(), std::back_inserter(buffer));
    buffer.push_back(message.recipient_login.size());
    std::copy(message.recipient_login.begin(), message.recipient_login.end(), std::back_inserter(buffer));
    std::string strPayload = json::serialize(PayloadSerializer::PayloadToJson(message.payload));
    // json size < UDP message size < 65000 = 2 bytes
    for (int i = 0; i < 2; ++i) {
        buffer.push_back((strPayload.size() >> (i * 8)) & 0xff);
    }
    std::copy(strPayload.begin(), strPayload.end(), std::back_inserter(buffer));
    return buffer;
}
