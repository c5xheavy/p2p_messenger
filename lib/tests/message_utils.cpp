#include "message_utils.h"

#include <ostream>

#include "payload_utils.h"

std::ostream& operator<<(std::ostream& out, const Message& message) {
    out << message.id << '\n';
    out << message.recipient_login << '\n';
    out << message.sender_login << '\n';
    out << message.payload << '\n';
    return out;
}

bool operator==(const Message& a, const Message& b) {
    if (a.id != b.id) return false;
    if (a.recipient_login != b.recipient_login) return false;
    if (a.sender_login != b.sender_login) return false;
    if (a.payload != b.payload) return false;
    return true;
}
