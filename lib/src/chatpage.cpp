#include "chatpage.h"
#include "ui_chatpage.h"

#include <iostream>
#include <memory>

#ifdef emit
#undef emit
#include <syncstream>
#define emit
#endif

#include "p2p_messenger_impl.h"

ChatPage::ChatPage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatPage) {
    ui->setupUi(this);
}

ChatPage::~ChatPage() {
    delete ui;
}

void ChatPage::ReceivedLoginParameters(const std::string& login, std::uint16_t dht_port, const std::string& ip, std::uint16_t port) {
    std::osyncstream(std::cout) << "Successful login!" << std::endl;
    std::osyncstream(std::cout) << "Login: " << login << std::endl;
    std::osyncstream(std::cout) << "DHT Port: " << dht_port << std::endl;
    std::osyncstream(std::cout) << "IP: " << ip << std::endl;
    std::osyncstream(std::cout) << "Port: " << port << std::endl;
    p2p_messenger_impl_ = std::make_shared<P2PMessengerImpl>(login, dht_port, ip, port,
        [this](const std::string& source_login, const std::string& message) {
            std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Received message from " << source_login << ": " << message << std::endl;
            ui->chatTextEdit->append(QString::fromStdString(source_login + ": " + message));
        },
        [this](const std::string& source_login, const std::string& message) {
            std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Send message: " << message << std::endl;
            ui->chatTextEdit->append(QString::fromStdString(source_login + ": " + message));
        },
        [this](const std::string& login, const std::string& address) {
            std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Received address for login " << login << ": " << address << std::endl;
            std::string prev_login = ui->destinationLoginLabel->text().toStdString();
            if (prev_login.empty() || prev_login == login) {
                std::osyncstream(std::cout) << "Updating destination address" << std::endl;
                ui->destinationAddressLabel->setText(QString::fromStdString(address));
            }
        }
    );
    std::osyncstream(std::cout) << "P2P Messenger created!" << std::endl;
    emit LogIn();
}

void ChatPage::on_logoutPushButton_clicked() {
    std::osyncstream(std::cout) << "Loging out!" << std::endl;
    p2p_messenger_impl_.reset();
    emit LogOut();
}

void ChatPage::on_sendPushButton_clicked() {
    std::osyncstream(std::cout) << "Send button clicked!" << std::endl;
    std::string login = ui->destinationLoginLabel->text().toStdString();
    if (login.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination login is empty" << std::endl;
        return;
    }
    std::string message = ui->messageLineEdit->text().toStdString();
    if (message.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Message is empty" << std::endl;
        return;
    }
    ui->messageLineEdit->clear();
    p2p_messenger_impl_->on_send_message(login, message);
}


void ChatPage::on_messageLineEdit_returnPressed() {
    std::osyncstream(std::cout) << "Return pressed!" << std::endl;
    std::string login = ui->destinationLoginLabel->text().toStdString();
    if (login.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination login is empty" << std::endl;
        return;
    }
    std::string message = ui->messageLineEdit->text().toStdString();
    if (message.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Message is empty" << std::endl;
        return;
    }
    ui->messageLineEdit->clear();
    p2p_messenger_impl_->on_send_message(login, message);
}

void ChatPage::on_searchLoginPushButton_clicked() {
    std::osyncstream(std::cout) << "Search button clicked!" << std::endl;
    std::string login = ui->searchLoginLineEdit->text().toStdString();
    if (login.empty()) {
        std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Login is empty" << std::endl;
        return;
    }
    ui->searchLoginLineEdit->clear();
    ui->destinationLoginLabel->setText(QString::fromStdString(login));
    p2p_messenger_impl_->on_listen(login);
    std::optional<std::string> address = p2p_messenger_impl_->on_search_login(login);
    if (address) {
        ui->destinationAddressLabel->setText(QString::fromStdString(*address));
    } else {
        ui->destinationAddressLabel->setText("Not found");
    }
}

