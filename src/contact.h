#ifndef CONTACT_H
#define CONTACT_H

#include <string>

#include <opendht.h>

struct Contact {
    std::string login;
    dht::InfoHash public_key_id;
};

#endif // CONTACT_H
