#include "message_utils.h"

#include <ostream>

#include "payload_utils.h"

std::ostream& operator<<(std::ostream& out, const Message& message) {
    out << message.id << '\n';
    out << message.source_login_hash << '\n';
    out << message.destination_login_hash << '\n';
    out << message.payload << '\n';
    return out;
}

bool operator==(const Message& a, const Message& b) {
    if (a.id != b.id) return false;
    if (a.source_login_hash != b.source_login_hash) return false;
    if (a.destination_login_hash != b.destination_login_hash) return false;
    if (a.payload != b.payload) return false;
    return true;
}
