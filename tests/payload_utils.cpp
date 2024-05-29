#include "payload_utils.h"

#include <ostream>

std::ostream& operator<<(std::ostream& out, const Payload& payload) {
    out << payload.time << std::endl;
    out << payload.text << std::endl;
    return out;
}

bool operator==(const Payload& a, const Payload& b) {
    if (a.time != b.time) return false;
    if (a.text != b.text) return false;
    return true;
}
