#ifndef DATAGRAM_DESERIALIZER_H
#define DATAGRAM_DESERIALIZER_H

#include <string>
#include <vector>

#include "datagram.h"

class DatagramDeserializer {
public:
    static Datagram DatagramFromBuffer(const Buffer& buffer);
};

#endif
