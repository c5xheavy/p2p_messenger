#ifndef PAYLOAD_UTILS_H
#define PAYLOAD_UTILS_H

#include "../src/payload.h"

std::ostream& operator<<(std::ostream& out, const Payload& payload);

bool operator==(const Payload& a, const Payload& b);

#endif // PAYLOAD_UTILS_H
