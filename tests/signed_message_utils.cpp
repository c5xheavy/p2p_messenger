#include "signed_message_utils.h"

#include <ostream>

#include "payload_utils.h"

std::ostream& operator<<(std::ostream& out, const SignedMessage& signed_message) {
    for (const auto& x : signed_message.message) {
        out << x << ' ';
    }
    out << std::endl;
    for (const auto& x : signed_message.signature) {
        out << x << ' ';
    }
    out << std::endl;
    return out;
}

bool operator==(const SignedMessage& a, const SignedMessage& b) {
    if (a.message != b.message) return false;
    if (a.signature != b.signature) return false;
    return true;
}
