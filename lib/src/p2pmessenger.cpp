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
    , ui(new Ui::P2PMessenger)
{
    ui->setupUi(this);
    connect(ui->loginPage, &LoginPage::on_Login_LoginPage, this, [this](const std::string& login, std::uint16_t dht_port, const std::string& ip, std::uint16_t port) {
        on_Login_P2PMessenger(login, dht_port, ip, port);
    });
    connect(this, &P2PMessenger::on_SuccessfulLogin_P2PMessenger, ui->chatPage, [this](const std::string& login, std::uint16_t dht_port, const std::string& ip, std::uint16_t port) {
        ui->chatPage->on_SuccessfulLogin_ChatPage(login, dht_port, ip, port);
    });
    connect(ui->chatPage, &ChatPage::on_AfterSuccessfulLogin_ChatPage, this, &P2PMessenger::on_AfterSuccessfulLogin_P2PMessenger);
    connect(ui->chatPage, &ChatPage::on_Logout_ChatPage, this, &P2PMessenger::on_Logout_P2PMessenger);
}

P2PMessenger::~P2PMessenger()
{
    delete ui;
}

void P2PMessenger::on_Login_P2PMessenger(const std::string& login, std::uint16_t dht_port, const std::string& ip, std::uint16_t port)
{
    std::osyncstream(std::cout) << "Login: " << login << std::endl;
    std::osyncstream(std::cout) << "DHT Port: " << dht_port << std::endl;
    std::osyncstream(std::cout) << "IP: " << ip << std::endl;
    std::osyncstream(std::cout) << "Port: " << port << std::endl;
    emit on_SuccessfulLogin_P2PMessenger(login, dht_port, ip, port);
    std::osyncstream(std::cout) << "After emit" << std::endl;
}

void P2PMessenger::on_AfterSuccessfulLogin_P2PMessenger() {
    std::osyncstream(std::cout) << "After successful login" << std::endl;
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
}

void P2PMessenger::on_Logout_P2PMessenger() {
    std::cout << "Logging out!" << std::endl;
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
}
