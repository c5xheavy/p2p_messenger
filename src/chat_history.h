#ifndef CHAT_HISTORY_H
#define CHAT_HISTORY_H

#include <map>
#include <mutex>
#include <list>
#include <string>

#include <opendht.h>

class ChatHistory {
public:
    void append(const dht::InfoHash& public_key_id, const std::string& login, const std::string& message);
    const std::list<std::pair<std::string, std::string>>* get(const dht::InfoHash& public_key_id) const;

private:
    std::map<dht::InfoHash, std::list<std::pair<std::string, std::string>>> chat_history_;
    mutable std::mutex chat_history_mutex_;
};

#endif // CHAT_HISTORY_H
