#include "datagram_serializer.h"
#include "message_serializer.h"

#include <iostream>

Buffer DatagramSerializer::DatagramToBuffer(const Datagram& datagram) {
    Buffer buffer;
    for (int i = 0; i < 8; ++i) {
        // is it UB?
        // buffer.push_back(static_cast<std::uint8_t>(datagram.id >> (i * 8)));
        buffer.push_back((datagram.id >> (i * 8)) & 0xff);
    }
    // login size <= 64 = 1 byte
    buffer.push_back(datagram.sender_login.size());
    for (const char c : datagram.sender_login) {
        buffer.push_back(c);
    }
    buffer.push_back(datagram.recipient_login.size());
    for (const char c : datagram.recipient_login) {
        buffer.push_back(c);
    }
    std::string strMessage = json::serialize(MessageSerializer::MessageToJson(datagram.message));
    // json size < UDP datagram size < 65000 = 2 bytes
    for (int i = 0; i < 2; ++i) {
        buffer.push_back((strMessage.size() >> (i * 8)) & 0xff);
    }
    for (const char c : strMessage) {
        buffer.push_back(c);
    }
    return buffer;
}
