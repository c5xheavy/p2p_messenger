#ifndef ADDRESS_H
#define ADDRESS_H

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string>
#include <utility>

class Address {
public:
    Address() : ip_{}, port_{0}, set_(false) {}

    std::pair<std::string, std::uint16_t> get() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return {ip_, port_};
    }

    std::pair<std::string, std::uint16_t> getWait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return set_; });
        return {ip_, port_};
    }

    void set(const std::string& ip, const std::uint16_t& port) {
        std::lock_guard<std::mutex> lock(mutex_);
        ip_ = ip;
        port_ = port;
        set_ = true;
        cv_.notify_all();
    }

    void set(const std::string& address) {
        std::size_t pos = address.find(':');
        if (pos == std::string::npos) {
            throw std::invalid_argument{"Invalid address"};
        }
        set(address.substr(0, pos), std::stoi(address.substr(pos + 1)));
    }

private:
    std::string ip_;
    std::uint16_t port_;
    bool set_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
};

#endif  // ADDRESS_H
