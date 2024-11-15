#include "MessageBase.h"
std::string MessageBase::generateMessageId() {
    return std::to_string(std::time(nullptr)) + std::to_string(rand());
}

