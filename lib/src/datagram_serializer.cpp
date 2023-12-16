#include "datagram_serializer.h"
#include "payload_serializer.h"

#include <algorithm>

Buffer DatagramSerializer::DatagramToBuffer(const Datagram& datagram) {
    Buffer buffer;
    for (int i = 0; i < 8; ++i) {
        // is it UB?
        // buffer.push_back(static_cast<std::uint8_t>(datagram.id >> (i * 8)));
        buffer.push_back((datagram.id >> (i * 8)) & 0xff);
    }
    // login size <= 64 = 1 byte
    buffer.push_back(datagram.sender_login.size());
    std::copy(datagram.sender_login.begin(), datagram.sender_login.end(), std::back_inserter(buffer));
    buffer.push_back(datagram.recipient_login.size());
    std::copy(datagram.recipient_login.begin(), datagram.recipient_login.end(), std::back_inserter(buffer));
    std::string strPayload = json::serialize(PayloadSerializer::PayloadToJson(datagram.payload));
    // json size < UDP datagram size < 65000 = 2 bytes
    for (int i = 0; i < 2; ++i) {
        buffer.push_back((strPayload.size() >> (i * 8)) & 0xff);
    }
    std::copy(strPayload.begin(), strPayload.end(), std::back_inserter(buffer));
    return buffer;
}
