#pragma once

#include <string>

#include "message.h"

struct Datagram {
    std::string sender_login;
    std::string recipient_login;
    unsigned long long id;
    Message data;
};