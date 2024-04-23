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
    Ui::P2PMessenger *ui_;

    std::uint16_t dht_port_; 
    std::string my_ip_;
    std::uint16_t my_port_; 
    std::string my_login_;
    std::string destination_login_;

    const std::size_t num_threads_;

    net::io_context io_context_;

    net::executor_work_guard<net::io_context::executor_type> work_guard_;

    DhtIpResolver dht_ip_resolver_;

    std::vector<std::jthread> threads_;

    MessageReceiver message_receiver_;

    MessageSender message_sender_;
};

#endif // P2P_MESSENGER_H
