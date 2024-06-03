#include "chatpage.h"
#include "ui_chatpage.h"

#include <iostream>
#include <memory>

#ifdef emit
#undef emit
#include <syncstream>
#define emit
#endif

#include "message.h"
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

void ChatPage::log_in(const std::string& login, uint16_t dht_port, const std::string& ip,
                      uint16_t port, bool generate_crypto_identity, const std::string& crypto_identity_path) {
    std::osyncstream(std::cout) << "Successful login!" << std::endl;
    std::osyncstream(std::cout) << "Login: " << login << std::endl;
    std::osyncstream(std::cout) << "DHT Port: " << dht_port << std::endl;
    std::osyncstream(std::cout) << "IP: " << ip << std::endl;
    std::osyncstream(std::cout) << "Port: " << port << std::endl;
    std::osyncstream(std::cout) << "Generate crypto identity: " << generate_crypto_identity << std::endl;
    std::osyncstream(std::cout) << "Crypto identity path: " << crypto_identity_path << std::endl;
    p2p_messenger_impl_ = std::make_shared<P2PMessengerImpl>(login, dht_port, ip, port, generate_crypto_identity, crypto_identity_path,
        std::bind(&ChatPage::send_message_handler, this, std::placeholders::_1),
        std::bind(&ChatPage::receive_message_handler, this, std::placeholders::_1),
        std::bind(&ChatPage::listen_login_handler, this, std::placeholders::_1, std::placeholders::_2)
    );
    connect(this, &ChatPage::message_sent, this, &ChatPage::update_chat_with_sent_message);
    connect(this, &ChatPage::message_received, this, &ChatPage::update_chat_with_received_message);
    connect(this, &ChatPage::contact_received, this, &ChatPage::update_contacts_list_with_received_contact);
    std::osyncstream(std::cout) << "P2P Messenger created!" << std::endl;
    emit logged_in();
}

void ChatPage::send_message_handler(Message&& message) {
    std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "SendMessageHandler called" << std::endl;
    emit message_sent(std::make_shared<Message>(std::move(message)));
}

void ChatPage::receive_message_handler(Message&& message) {
    std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "ReceiveMessageHandler called" << std::endl;
    emit message_received(std::make_shared<Message>(std::move(message)));
}

void ChatPage::listen_login_handler(std::string&& login, dht::InfoHash&& public_key_id) {
    std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "ListenLoginHandler called" << std::endl;
    emit contact_received(std::make_shared<std::string>(std::move(login)), std::make_shared<dht::InfoHash>(std::move(public_key_id)));
}

void ChatPage::update_chat_with_sent_message(std::shared_ptr<Message> message) {
    std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "update_chat_with_sent_message called" << std::endl;
    ui_->chatTextEdit->append(QString::fromStdString(message->source_login + ": " + message->payload.text));
}

void ChatPage::update_chat_with_received_message(std::shared_ptr<Message> message) {
    std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "update_chat_with_received_message called" << std::endl;
    ui_->chatTextEdit->append(QString::fromStdString(message->destination_login + ": " + message->payload.text));
}

void ChatPage::update_contacts_list_with_received_contact(std::shared_ptr<std::string> login, std::shared_ptr<dht::InfoHash> public_key_id) {
    std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "update_contacts_list_with_received_contact called" << std::endl;
    if (*public_key_id) {
        QString item{QString::fromStdString(login_and_public_key_id_to_contact(*login, *public_key_id))};
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

void ChatPage::send_message() {
    if (!ui_->contactsListWidget->currentItem()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "No contact is selected" << std::endl;
        return;
    }
    auto [login, public_key_id]{contact_to_login_and_public_key_id(ui_->contactsListWidget->currentItem()->text().toStdString())};
    std::string message = ui_->messageLineEdit->text().toStdString();
    if (message.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Message is empty" << std::endl;
        return;
    }
    ui_->messageLineEdit->clear();
    p2p_messenger_impl_->send_message(login, public_key_id, message);
}

void ChatPage::on_sendPushButton_clicked() {
    std::osyncstream(std::cout) << "Send button clicked!" << std::endl;
    send_message();
}


void ChatPage::on_messageLineEdit_returnPressed() {
    std::osyncstream(std::cout) << "Return pressed!" << std::endl;
    send_message();
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
    std::optional<std::string> address = p2p_messenger_impl_->resolve(login, public_key_id);
    if (address) {
        ui_->destinationAddressLabel->setText(QString::fromStdString(*address));
    } else {
        ui_->destinationAddressLabel->setText("Not found");
    }
}

std::pair<std::string, dht::InfoHash> ChatPage::contact_to_login_and_public_key_id(const std::string& contact) {
    size_t start = contact.find('[');
    size_t end = contact.find(']');

    if (start == std::string::npos || end == std::string::npos || start >= end) {
        throw std::invalid_argument("Invalid contact format");
    }

    std::string public_key_str = contact.substr(start + 1, end - start - 1);
    std::string login = contact.substr(end + 2); // +2 to skip '] ' (closing bracket and space)

    dht::InfoHash public_key(public_key_str);

    return {login, public_key};
}

std::string ChatPage::login_and_public_key_id_to_contact(const std::string& login, const dht::InfoHash& public_key_id) {
    return "[" + public_key_id.toString() + "] " + login;
}
