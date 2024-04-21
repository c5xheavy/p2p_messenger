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
#include <utility>
#include <vector>

#include <boost/asio.hpp>
#include <opendht.h>

#include "dht_ip_resolver.h"
#include "message.h"
#include "message_receiver.h"
#include "message_sender.h"
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

private:
    void on_send_message();

private slots:
    void on_sendButton_clicked();

    void on_messageEdit_returnPressed();

private:
    Ui::P2PMessenger *ui;

    std::uint16_t dht_port; 
    std::string my_ip;
    std::uint16_t my_port; 
    std::string my_login;
    std::string destination_login;

    DhtIpResolver dht_ip_resolver;

    const std::size_t num_threads;

    net::io_context io_context;

    net::executor_work_guard<net::io_context::executor_type> work_guard;

    std::vector<std::jthread> threads;

    MessageReceiver message_receiver;

    MessageSender message_sender;
};

#endif // P2P_MESSENGER_H
