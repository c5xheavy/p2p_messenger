#ifndef MESSAGE_DESERIALIZER_H
#define MESSAGE_DESERIALIZER_H

#include <string>

#include <boost/json.hpp>

#include "message.h"

namespace json = boost::json;

class MessageDeserializer {
public:
    static Message MessageFromJson(const json::object& jsonMessage);
};

#endif
