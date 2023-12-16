#ifndef DATAGRAM_SERIALIZER_H
#define DATAGRAM_SERIALIZER_H

#include <string>
#include <vector>

#include "datagram.h"

class DatagramSerializer {
public:
    static Buffer DatagramToBuffer(const Datagram& datagram);
};

#endif
