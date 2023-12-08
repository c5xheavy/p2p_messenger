#pragma once

#include "../src/message.h"

std::ostream& operator<<(std::ostream& out, const Message& message);

bool operator==(const Message& a, const Message& b);