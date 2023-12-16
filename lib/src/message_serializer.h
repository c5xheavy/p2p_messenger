#ifndef MESSAGE_SERIALIZER_H
#define MESSAGE_SERIALIZER_H

#include <string>

#include <boost/json.hpp>

#include "message.h"

namespace json = boost::json;

class MessageSerializer {
public:
    static json::object MessageToJson(const Message& message);
};

#endif
