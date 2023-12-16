#include "datagram_deserializer.h"
#include "payload_deserializer.h"

Datagram DatagramDeserializer::DatagramFromBuffer(const Buffer& buffer) {
    Datagram datagram{};
    std::size_t i = 0;
    for (std::size_t j = 0; j < 8; ++j) {
        datagram.id |= static_cast<std::uint64_t>(buffer[i]) << (j * 8);
        ++i;
    }
    std::uint8_t sender_login_size = buffer[i++];
    for (std::uint8_t j = 0; j < sender_login_size; ++j) {
        datagram.sender_login.push_back(buffer[i++]);
    }
    std::uint8_t recipient_login_size = buffer[i++];
    for (std::uint8_t j = 0; j < recipient_login_size; ++j) {
        datagram.recipient_login.push_back(buffer[i++]);
    }
    std::uint16_t payload_size = 0;
    for (std::size_t j = 0; j < 2; ++j) {
        payload_size |= static_cast<std::uint16_t>(buffer[i]) << (j * 8);
        ++i;
    }
    std::string strPayload;
    for (std::uint16_t j = 0; j < payload_size; ++j) {
        strPayload.push_back(buffer[i++]);
    }
    datagram.payload = PayloadDeserializer::PayloadFromJson(json::parse(strPayload).as_object());
    return datagram;
}
