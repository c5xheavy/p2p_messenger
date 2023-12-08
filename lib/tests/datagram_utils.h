#pragma once

#include "../src/datagram.h"

std::ostream& operator<<(std::ostream& out, const Datagram& datagram);

bool operator==(const Datagram& a, const Datagram& b);