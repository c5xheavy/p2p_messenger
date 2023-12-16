#ifndef DATAGRAM_UTILS_H
#define DATAGRAM_UTILS_H

#include "../src/datagram.h"

std::ostream& operator<<(std::ostream& out, const Datagram& datagram);

bool operator==(const Datagram& a, const Datagram& b);

#endif
