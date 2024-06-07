#include "message_utils.h"

#include <ostream>

#include "payload_utils.h"

std::ostream& operator<<(std::ostream& out, const Message& message) {
    out << message.id << std::endl;
    out << message.source_ip << std::endl;
    out << message.source_port << std::endl;
    out << message.source_login << std::endl;
    out << message.source_public_key << std::endl;
    out << message.destination_ip << std::endl;
    out << message.destination_port << std::endl;
    out << message.destination_login << std::endl;
    out << message.destination_public_key << std::endl;
    out << message.payload << std::endl;
    return out;
}

bool operator==(const Message& a, const Message& b) {
    if (a.id != b.id) return false;
    if (a.source_ip != b.source_ip) return false;
    if (a.source_port != b.source_port) return false;
    if (a.source_login != b.source_login) return false;
    if (a.source_public_key != b.source_public_key) return false;
    if (a.destination_ip != b.destination_ip) return false;
    if (a.destination_port != b.destination_port) return false;
    if (a.destination_login != b.destination_login) return false;
    if (a.destination_public_key != b.destination_public_key) return false;
    if (a.payload != b.payload) return false;
    return true;
}
