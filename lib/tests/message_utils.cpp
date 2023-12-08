#include "message_utils.h"

#include <ostream>

std::ostream& operator<<(std::ostream& out, const Message& message) {
    out << message.sender_ip << '\n';
    out << message.time << '\n';
    out << message.text << '\n';
    return out;
}

bool operator==(const Message& a, const Message& b) {
    if (a.sender_ip != b.sender_ip) return false;
    if (a.time != b.time) return false;
    if (a.text != b.text) return false;
    return true;
}