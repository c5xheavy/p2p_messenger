#include "payload_utils.h"

#include <ostream>

std::ostream& operator<<(std::ostream& out, const Payload& payload) {
    out << payload.time << '\n';
    out << payload.text << '\n';
    return out;
}

bool operator==(const Payload& a, const Payload& b) {
    if (a.time != b.time) return false;
    if (a.text != b.text) return false;
    return true;
}
