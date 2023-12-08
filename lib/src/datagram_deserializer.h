#pragma once

#include <string>
#include <vector>

#include "datagram.h"

class DatagramDeserializer {
public:
    static Datagram DatagramFromBuffer(const Buffer& buffer);
};
