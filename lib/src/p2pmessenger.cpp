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
    connect(ui_->loginPage, &LoginPage::log_in_requsted, ui_->chatPage, &ChatPage::log_in);
    connect(ui_->chatPage, &ChatPage::logged_in, this, &P2PMessenger::switch_to_chat_page);
    connect(ui_->chatPage, &ChatPage::logged_out, this, &P2PMessenger::switch_to_login_page);
}

P2PMessenger::~P2PMessenger() {
    delete ui_;
}

void P2PMessenger::switch_to_chat_page() {
    std::osyncstream(std::cout) << "After successful login" << std::endl;
    ui_->stackedWidget->setCurrentWidget(ui_->chatPage);
}

void P2PMessenger::switch_to_login_page() {
    std::cout << "Logging out!" << std::endl;
    ui_->stackedWidget->setCurrentWidget(ui_->loginPage);
}
