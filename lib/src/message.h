#pragma once

#include <string>

struct Message {
    std::string sender_ip;
    int64_t time;
    std::string text;
};
