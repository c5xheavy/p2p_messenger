#ifndef SIGNED_MESSAGE_UTILS_H
#define SIGNED_MESSAGE_UTILS_H

#include "../src/signed_message.h"

std::ostream& operator<<(std::ostream& out, const SignedMessage& signed_message);

bool operator==(const SignedMessage& a, const SignedMessage& b);

#endif // SIGNED_MESSAGE_UTILS_H
