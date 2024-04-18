#ifndef P2P_MESSENGER_H
#define P2P_MESSENGER_H

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <syncstream>
#include <thread>
#include <vector>

#include <boost/asio.hpp>
#include <opendht.h>

#include "dht_ip_resolver.h"
#include "message.h"
#include "message_serializer.h"
#include "message_deserializer.h"

#include <QMainWindow>

namespace net = boost::asio;
using net::ip::udp;
namespace sys = boost::system;

QT_BEGIN_NAMESPACE
namespace Ui { class P2PMessenger; }
QT_END_NAMESPACE

class P2PMessenger : public QMainWindow
{
    Q_OBJECT

public:
    P2PMessenger(QWidget *parent = nullptr);
    ~P2PMessenger();

    std::pair<std::string, std::uint16_t> get_ip_and_port_from_address(const std::string& address);

    void send_message(udp::socket& socket, const std::string& destination_login, const std::string& text);

private:
    void on_send_message();

private slots:
    void on_sendButton_clicked();

    void on_messageEdit_returnPressed();

private:
    Ui::P2PMessenger *ui;

    std::uint16_t dht_port = 3001;
    std::string my_ip = "127.0.0.1";
    std::uint16_t my_port = 3101;
    std::string my_login = "a1";
    std::string destination_login = "b1";

    DhtIpResolver dht_ip_resolver{dht_port};

    const unsigned num_threads;

    net::io_context io_context;

    net::executor_work_guard<net::io_context::executor_type> work_guard;

    std::vector<std::thread> threads;

    udp::socket receive_socket;

    udp::socket send_socket;

    std::function<void(const sys::error_code&)> async_wait_handler = [this](const sys::error_code& ec) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Got message" << std::endl;
        if (!ec) {
            std::size_t bytes_available{receive_socket.available()};
            std::shared_ptr<char[]> buffer{new char[bytes_available]};

            udp::endpoint remote_endpoint;
            std::size_t buffer_size{receive_socket.receive_from(net::buffer(buffer.get(), bytes_available), remote_endpoint)};

            if (bytes_available != buffer_size) {
                throw std::logic_error{"Bytes available is not equal bytes read"};
            }

            Message message{MessageDeserializer::MessageFromBuffer(buffer.get(), buffer_size)};

            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Received message:" << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.id << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.source_login << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.destination_login << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.payload.time << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << message.payload.text << std::endl;
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "End of message" << std::endl;
        } else {
            std::osyncstream(std::cerr) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Wait message error: " << ec.what() << std::endl;
        }


        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Waiting for next message" << std::endl;
        receive_socket.async_wait(udp::socket::wait_read, async_wait_handler);
    };
};

#endif // P2P_MESSENGER_H
