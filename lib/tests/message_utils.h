#ifndef DATAGRAM_UTILS_H
#define DATAGRAM_UTILS_H

#include "../src/message.h"

std::ostream& operator<<(std::ostream& out, const Message& message);

bool operator==(const Message& a, const Message& b);

#endif
