#include "message_utils.h"

#include <ostream>

#include "payload_utils.h"

std::ostream& operator<<(std::ostream& out, const Message& message) {
    out << message.id << std::endl;
    out << message.source_login << std::endl;
    out << message.destination_login << std::endl;
    out << message.payload << std::endl;
    return out;
}

bool operator==(const Message& a, const Message& b) {
    if (a.id != b.id) return false;
    if (a.source_login != b.source_login) return false;
    if (a.destination_login != b.destination_login) return false;
    if (a.payload != b.payload) return false;
    return true;
}
