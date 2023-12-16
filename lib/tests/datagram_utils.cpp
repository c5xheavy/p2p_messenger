#include "datagram_utils.h"

#include <ostream>

#include "payload_utils.h"

std::ostream& operator<<(std::ostream& out, const Datagram& datagram) {
    out << datagram.id << '\n';
    out << datagram.sender_login << '\n';
    out << datagram.recipient_login << '\n';
    out << datagram.payload << '\n';
    return out;
}

bool operator==(const Datagram& a, const Datagram& b) {
    if (a.id != b.id) return false;
    if (a.sender_login != b.sender_login) return false;
    if (a.recipient_login != b.recipient_login) return false;
    if (a.payload != b.payload) return false;
    return true;
}
