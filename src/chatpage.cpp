#include "chatpage.h"
#include "ui_chatpage.h"

#include <iostream>
#include <memory>

#ifdef emit
#undef emit
#include <syncstream>
#define emit
#endif

#include "chat_history.h"
#include "contact.h"
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
                      uint16_t port, const std::string& bootstrap_node_ip, uint16_t bootstrap_node_port, 
                      bool generate_crypto_identity, const std::string& crypto_identity_path) {
    std::osyncstream(std::cout) << "Successful login!" << std::endl;
    std::osyncstream(std::cout) << "Login: " << login << std::endl;
    std::osyncstream(std::cout) << "DHT Port: " << dht_port << std::endl;
    std::osyncstream(std::cout) << "IP: " << ip << std::endl;
    std::osyncstream(std::cout) << "Port: " << port << std::endl;
    std::osyncstream(std::cout) << "Bootstrap node IP: " << bootstrap_node_ip << std::endl;
    std::osyncstream(std::cout) << "Bootstrap node port: " << bootstrap_node_port << std::endl;
    std::osyncstream(std::cout) << "Generate crypto identity: " << generate_crypto_identity << std::endl;
    std::osyncstream(std::cout) << "Crypto identity path: " << crypto_identity_path << std::endl;
    p2p_messenger_impl_ = std::make_shared<P2PMessengerImpl>(login, dht_port, ip, port, bootstrap_node_ip, bootstrap_node_port, generate_crypto_identity, crypto_identity_path,
        std::bind(&ChatPage::send_message_handler, this, std::placeholders::_1),
        std::bind(&ChatPage::receive_message_handler, this, std::placeholders::_1),
        std::bind(&ChatPage::listen_login_handler, this, std::placeholders::_1)
    );
    connect(this, &ChatPage::message_sent, this, &ChatPage::update_chat_with_sent_message, Qt::UniqueConnection);
    connect(this, &ChatPage::message_received, this, &ChatPage::update_chat_with_received_message, Qt::UniqueConnection);
    connect(this, &ChatPage::contact_received, this, &ChatPage::update_contacts_list_with_received_contact, Qt::UniqueConnection);
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

void ChatPage::listen_login_handler(Contact&& contact) {
    std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "ListenLoginHandler called" << std::endl;
    emit contact_received(std::make_shared<Contact>(std::move(contact)));
}

void ChatPage::update_chat_with_sent_message(std::shared_ptr<Message> message) {
    std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "update_chat_with_sent_message called" << std::endl;
    ui_->chatTextEdit->append(QString::fromStdString(message->source_login + ": " + message->payload.text));
}

void ChatPage::update_chat_with_received_message(std::shared_ptr<Message> message) {
    std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "update_chat_with_received_message called" << std::endl;
    if (!ui_->contactsListWidget->currentItem()) {
        notify(message);
        return;
    }
    Contact contact{contact_from_string(ui_->contactsListWidget->currentItem()->text().toStdString())};
    dht::crypto::PublicKey public_key{message->source_public_key};
    if (contact.public_key_id != public_key.getId()) {
        notify(message);
        return;
    }
    ui_->chatTextEdit->append(QString::fromStdString(message->source_login + ": " + message->payload.text));
}

void ChatPage::update_contacts_list_with_received_contact(std::shared_ptr<Contact> contact) {
    std::cout << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "update_contacts_list_with_received_contact called" << std::endl;
    if (contact->public_key_id) {
        QString item{QString::fromStdString(contact_to_string(*contact))};
        if (ui_->contactsListWidget->findItems(item, Qt::MatchExactly).empty()) {
            ui_->contactsListWidget->addItem(item);
        }
        if (ui_->contactsListWidget->currentItem() && item == ui_->contactsListWidget->currentItem()->text()) {
            std::optional<std::string> address = p2p_messenger_impl_->resolve(contact->login, contact->public_key_id);
            if (address) {
                ui_->destinationAddressLabel->setText(QString::fromStdString(*address));
            } else {
                ui_->destinationAddressLabel->setText("Not found");
            }
        }
    } else {
        std::osyncstream(std::cout) << "Public key ID is null" << std::endl;
    }
}

void ChatPage::on_logoutPushButton_clicked() {
    std::osyncstream(std::cout) << "Loging out!" << std::endl;
    p2p_messenger_impl_.reset();
    ui_->contactsListWidget->clear();
    ui_->chatTextEdit->clear();
    ui_->destinationLoginLabel->clear();
    ui_->destinationAddressLabel->clear();
    ui_->messageLineEdit->clear();
    ui_->searchLoginLineEdit->clear();
    emit logged_out();
}

void ChatPage::send_message() {
    if (!ui_->contactsListWidget->currentItem()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "No contact is selected" << std::endl;
        return;
    }
    Contact contact{contact_from_string(ui_->contactsListWidget->currentItem()->text().toStdString())};
    std::string message = ui_->messageLineEdit->text().toStdString();
    if (message.empty()) {
        std::osyncstream(std::cout) << '[' << std::hash<std::thread::id>{}(std::this_thread::get_id()) << "] " << "Message is empty" << std::endl;
        return;
    }
    ui_->messageLineEdit->clear();
    p2p_messenger_impl_->send_message(contact.login, contact.public_key_id, message);
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
    Contact contact{contact_from_string(item->text().toStdString())};
    ui_->destinationLoginLabel->setText(QString::fromStdString(contact.login));
    std::optional<std::string> address = p2p_messenger_impl_->resolve(contact.login, contact.public_key_id);
    if (address) {
        ui_->destinationAddressLabel->setText(QString::fromStdString(*address));
    } else {
        ui_->destinationAddressLabel->setText("Not found");
    }
    QFont font = item->font();
    font.setBold(false);
    item->setFont(font);
    const std::list<std::pair<std::string, std::string>>* chat_history = p2p_messenger_impl_->get(contact.public_key_id);
    ui_->chatTextEdit->clear();
    if (chat_history) {
        for (const auto& message : *chat_history) {
            ui_->chatTextEdit->append(QString::fromStdString(message.first + ": " + message.second));
        }
    }
}

Contact ChatPage::contact_from_string(const std::string& str_contact) {
    size_t start = str_contact.find('[');
    size_t end = str_contact.find(']');

    if (start == std::string::npos || end == std::string::npos || start >= end) {
        throw std::invalid_argument("Invalid contact format");
    }

    std::string public_key_str = str_contact.substr(start + 1, end - start - 1);
    std::string login = str_contact.substr(end + 2); // +2 to skip '] ' (closing bracket and space)

    dht::InfoHash public_key_id(public_key_str);

    return {login, public_key_id};
}

std::string ChatPage::contact_to_string(const Contact& contact) {
    return "[" + contact.public_key_id.toString() + "] " + contact.login;
}

void ChatPage::notify(std::shared_ptr<Message> message) {
    std::osyncstream(std::cout) << "ChatPage::notify called" << std::endl;
    Contact contact{message->source_login, dht::crypto::PublicKey{message->source_public_key}.getId()};
    QString str_item = QString::fromStdString(contact_to_string(contact));
    QList<QListWidgetItem*> items = ui_->contactsListWidget->findItems(str_item, Qt::MatchExactly);
    if (items.empty()) {
        std::osyncstream(std::cout) << "ChatPage::notify git notified but there was no such contact" << std::endl;
        ui_->contactsListWidget->addItem(str_item);
    }
    QListWidgetItem* item = items.first();
    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);
}
