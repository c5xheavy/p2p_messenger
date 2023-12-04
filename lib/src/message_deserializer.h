#pragma once

#include <string>

#include <boost/json.hpp>

#include "message.h"

namespace json = boost::json;

class MessageDeserializer {
public:
    static Message MessageFromJson(const json::object& jsonMessage);
};
