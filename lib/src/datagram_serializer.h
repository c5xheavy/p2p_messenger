#pragma once

#include <string>
#include <vector>

#include "datagram.h"

class DatagramSerializer {
public:
    static Buffer DatagramToBuffer(const Datagram& datagram);
};
