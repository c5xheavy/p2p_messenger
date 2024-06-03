#include "chat_history.h"

#include <mutex>
#include <list>
#include <string>

#include <opendht.h>

void ChatHistory::append(const dht::InfoHash& public_key_id, const std::string& login, const std::string& message) {
    std::lock_guard<std::mutex> lock{chat_history_mutex_};
    chat_history_[public_key_id].emplace_back(login, message);
}

const std::list<std::pair<std::string, std::string>>* ChatHistory::get(const dht::InfoHash& public_key_id) const {
    std::lock_guard<std::mutex> lock{chat_history_mutex_};
    const auto it = chat_history_.find(public_key_id);
    if (it == chat_history_.end()) {
        return nullptr;
    }
    return &it->second;
}
