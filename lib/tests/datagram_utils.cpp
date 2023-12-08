#include "datagram_utils.h"

#include <ostream>

#include "message_utils.h"

std::ostream& operator<<(std::ostream& out, const Datagram& datagram) {
    out << datagram.id << '\n';
    out << datagram.sender_login << '\n';
    out << datagram.recipient_login << '\n';
    out << datagram.message << '\n';
    return out;
}

bool operator==(const Datagram& a, const Datagram& b) {
    if (a.id != b.id) return false;
    if (a.sender_login != b.sender_login) return false;
    if (a.recipient_login != b.recipient_login) return false;
    if (a.message != b.message) return false;
    return true;
}