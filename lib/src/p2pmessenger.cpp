#include "p2pmessenger.h"
#include "./ui_p2pmessenger.h"

#include <iostream>

#ifdef emit
#undef emit
#include <syncstream>
#define emit
#endif

P2PMessenger::P2PMessenger(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::P2PMessenger) {
    ui_->setupUi(this);
    connect(ui_->loginPage, &LoginPage::SendLoginParameters, this, &P2PMessenger::ReceivedLoginParameters);
    connect(this, &P2PMessenger::SendLoginParameters, ui_->chatPage, &ChatPage::ReceivedLoginParameters);
    connect(ui_->chatPage, &ChatPage::LogIn, this, &P2PMessenger::LoggedIn);
    connect(ui_->chatPage, &ChatPage::LogOut, this, &P2PMessenger::LoggedOut);
}

P2PMessenger::~P2PMessenger() {
    delete ui_;
}

void P2PMessenger::ReceivedLoginParameters(const std::string& login, std::uint16_t dht_port, const std::string& ip, std::uint16_t port) {
    std::osyncstream(std::cout) << "Login: " << login << std::endl;
    std::osyncstream(std::cout) << "DHT Port: " << dht_port << std::endl;
    std::osyncstream(std::cout) << "IP: " << ip << std::endl;
    std::osyncstream(std::cout) << "Port: " << port << std::endl;
    emit SendLoginParameters(login, dht_port, ip, port);
    std::osyncstream(std::cout) << "After emit" << std::endl;
}

void P2PMessenger::LoggedIn() {
    std::osyncstream(std::cout) << "After successful login" << std::endl;
    ui_->stackedWidget->setCurrentWidget(ui_->chatPage);
}

void P2PMessenger::LoggedOut() {
    std::cout << "Logging out!" << std::endl;
    ui_->stackedWidget->setCurrentWidget(ui_->loginPage);
}
