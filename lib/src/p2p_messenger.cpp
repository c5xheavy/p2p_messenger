#include "p2p_messenger.h"
#include "./ui_p2p_messenger.h"

#include <iostream>
#include <string>
#include <thread>
#include <utility>

// need this to avoid conflict with Qt's emit keyword
#ifdef emit
#undef emit
#include <syncstream>
#define emit
#endif

P2PMessenger::P2PMessenger(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::P2PMessenger)
    , dht_port_{3001}
    , my_ip_{"127.0.0.1"}
    , my_port_{3101}
    , my_login_{"a1"}
    , destination_login_{"b1"}
    , num_threads_{4}
    , io_context_{static_cast<int>(num_threads_)}
    , work_guard_{net::make_work_guard(io_context_)}
    , dht_ip_resolver_{io_context_, dht_port_}
    , threads_{}
    , message_receiver_{io_context_, my_port_, [this](const std::string& source_login, const std::string& message) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Received message from " << source_login << ": " << message << std::endl;
        ui_->chatHistoryView->append(QString::fromStdString(source_login + ": " + message));
    }}
    , message_sender_{io_context_, dht_ip_resolver_, my_login_, [this](const std::string& source_login, const std::string& message) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Received message from " << source_login << ": " << message << std::endl;
        ui_->chatHistoryView->append(QString::fromStdString(source_login + ": " + message));
    }} {
    ui_->setupUi(this);

    // put data on the dht
    dht_ip_resolver_.put(my_login_, my_ip_, my_port_);

    // listen for data on the dht
    dht_ip_resolver_.listen(destination_login_);
    dht_ip_resolver_.listen(my_login_);

    threads_.reserve(num_threads_);
    for (std::size_t i = 0; i < num_threads_; ++i) {
        threads_.emplace_back([this] {
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Running io_context thread" << std::endl;
            io_context_.run();
            std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopping io_context thread" << std::endl;
        });
    }
}

P2PMessenger::~P2PMessenger()
{
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "P2PMessenger destructor called" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Shutting down gracefully..." << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopping work_guard" << std::endl;
    work_guard_.reset();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopped work_guard" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopping io_context" << std::endl;
    io_context_.stop();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Stopped io_context" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Deleting ui" << std::endl;
    delete ui_;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Deleted ui" << std::endl;
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "P2PMessenger destructor finished" << std::endl;
}

void P2PMessenger::on_send_message() {
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "on_send_message called" << std::endl;
    std::string message = ui_->messageEdit->text().toStdString();
    if (message.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Message is empty" << std::endl;
        return;
    }
    ui_->messageEdit->clear();
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Sending message: " << message << std::endl;
    net::post(io_context_, [this, message]() {{
            std::optional<std::string> destination_address = dht_ip_resolver_.Resolve(destination_login_);
            if (destination_address) {
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is set" << std::endl;
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Sending message to " << destination_login_ << '\n';
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Calling send_message" << '\n';
                net::post(io_context_, [this, message]() {
                    message_sender_.send_message(destination_login_, message);
                });
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Called send_message" << '\n';
            } else {
                std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination address is not set" << std::endl;
            }
        }});
}

void P2PMessenger::on_sendButton_clicked()
{
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Send button is clicked" << std::endl;
    on_send_message();
}

void P2PMessenger::on_messageEdit_returnPressed()
{
    std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Return key is pressed" << std::endl;
    on_send_message();
}

void P2PMessenger::on_loginButton_clicked()
{
    ui_->stackedWidget->setCurrentIndex(1);
}

void P2PMessenger::on_logoutButton_clicked()
{
    ui_->stackedWidget->setCurrentIndex(0);
}
