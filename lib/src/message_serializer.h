#pragma once

#include <string>

#include <boost/json.hpp>

#include "message.h"

namespace json = boost::json;

class MessageSerializer {
public:
    static json::object MessageToJson(const Message& message);
};
