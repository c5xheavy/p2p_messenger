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
    ui_(new Ui::ChatPage) {
    ui_->setupUi(this);
    ui_->contactsListWidget->setSortingEnabled(true);
}

ChatPage::~ChatPage() {
    delete ui_;
}

void ChatPage::log_in(const std::string& login, std::uint16_t dht_port, const std::string& ip,
                      std::uint16_t port, bool generate_crypto_identity, const std::string& crypto_identity_path) {
    std::osyncstream(std::cout) << "Successful login!" << std::endl;
    std::osyncstream(std::cout) << "Login: " << login << std::endl;
    std::osyncstream(std::cout) << "DHT Port: " << dht_port << std::endl;
    std::osyncstream(std::cout) << "IP: " << ip << std::endl;
    std::osyncstream(std::cout) << "Port: " << port << std::endl;
    std::osyncstream(std::cout) << "Generate crypto identity: " << generate_crypto_identity << std::endl;
    std::osyncstream(std::cout) << "Crypto identity path: " << crypto_identity_path << std::endl;
    p2p_messenger_impl_ = std::make_shared<P2PMessengerImpl>(login, dht_port, ip, port, generate_crypto_identity, crypto_identity_path,
        [this](const std::string& login, const std::string& message) {
            std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Send message: " << message << std::endl;
            emit message_sent(login, message);
        },
        [this](const std::string& login, const std::string& message) {
            std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Received message from " << login << ": " << message << std::endl;
            emit message_received(login, message);
        },
        [this](const std::string& login, const dht::InfoHash& public_key_id) {
            std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Received contact " << login << ": " << public_key_id.toString() << std::endl;
            emit contact_received(login, public_key_id);
        }
    );
    connect(this, &ChatPage::message_sent, this, &ChatPage::update_chat_with_sent_message);
    connect(this, &ChatPage::message_received, this, &ChatPage::update_chat_with_received_message);
    connect(this, &ChatPage::contact_received, this, &ChatPage::update_contacts_list_with_received_contact);
    std::osyncstream(std::cout) << "P2P Messenger created!" << std::endl;
    emit logged_in();
}

void ChatPage::update_chat_with_sent_message(const std::string& login, const std::string message) {
    ui_->chatTextEdit->append(QString::fromStdString(login + ": " + message));
}

void ChatPage::update_chat_with_received_message(const std::string& login, const std::string& message) {
    ui_->chatTextEdit->append(QString::fromStdString(login + ": " + message));
}

void ChatPage::update_contacts_list_with_received_contact(const std::string& login, const dht::InfoHash& public_key_id) {
    std::osyncstream(std::cout) << "update_contacts_list_with_received_contact" << std::endl;
    if (public_key_id) {
        QString item{QString::fromStdString(login_and_public_key_id_to_contact(login, public_key_id))};
        if (ui_->contactsListWidget->findItems(item, Qt::MatchExactly).empty()) {
            ui_->contactsListWidget->addItem(item);
        }
    } else {
        std::osyncstream(std::cout) << "Public key ID is null" << std::endl;
    }
}

void ChatPage::on_logoutPushButton_clicked() {
    std::osyncstream(std::cout) << "Loging out!" << std::endl;
    p2p_messenger_impl_.reset();
    emit logged_out();
}

void ChatPage::on_sendPushButton_clicked() {
    std::osyncstream(std::cout) << "Send button clicked!" << std::endl;
    std::string login = ui_->destinationLoginLabel->text().toStdString();
    if (login.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination login is empty" << std::endl;
        return;
    }
    std::string message = ui_->messageLineEdit->text().toStdString();
    if (message.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Message is empty" << std::endl;
        return;
    }
    ui_->messageLineEdit->clear();
    p2p_messenger_impl_->send_message(login, message);
}


void ChatPage::on_messageLineEdit_returnPressed() {
    std::osyncstream(std::cout) << "Return pressed!" << std::endl;
    std::string login = ui_->destinationLoginLabel->text().toStdString();
    if (login.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Destination login is empty" << std::endl;
        return;
    }
    std::string message = ui_->messageLineEdit->text().toStdString();
    if (message.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Message is empty" << std::endl;
        return;
    }
    ui_->messageLineEdit->clear();
    p2p_messenger_impl_->send_message(login, message);
}

void ChatPage::on_searchLoginPushButton_clicked() {
    std::osyncstream(std::cout) << "Search button clicked!" << std::endl;
    std::string login = ui_->searchLoginLineEdit->text().toStdString();
    if (login.empty()) {
        std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Login is empty" << std::endl;
        return;
    }
    ui_->searchLoginLineEdit->clear();
    p2p_messenger_impl_->listen(login);
}

void ChatPage::on_contactsListWidget_itemClicked(QListWidgetItem *item) {
    auto [login, public_key_id]{contact_to_login_and_public_key_id(item->text().toStdString())};
    ui_->destinationLoginLabel->setText(QString::fromStdString(login));
    std::optional<std::string> address = p2p_messenger_impl_->resolve(login);
    if (address) {
        ui_->destinationAddressLabel->setText(QString::fromStdString(*address));
    } else {
        ui_->destinationAddressLabel->setText("Not found");
    }
}
