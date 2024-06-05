#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <memory>

#include <QDialog>
#include <QListWidgetItem>

#include "contact.h"
#include "chat_history.h"
#include "message.h"
#include "p2p_messenger_impl.h"

namespace Ui {
class ChatPage;
}

class ChatPage : public QDialog {
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();

    void send_message_handler(Message&& message);
    void receive_message_handler(Message&& message);
    void listen_login_handler(Contact&& contact);

private:
    void send_message();
    Contact contact_from_string(const std::string& str_contact);
    std::string contact_to_string(const Contact& contact);
    void notify(std::shared_ptr<Message> message);

signals:
    void logged_in();
    void logged_out();
    void message_sent(std::shared_ptr<Message> message);
    void message_received(std::shared_ptr<Message> message);
    void contact_received(std::shared_ptr<Contact> contact);

public slots:
    void log_in(const std::string& login, uint16_t dht_port, const std::string& ip,
                uint16_t port, const std::string& bootstrap_node_ip, uint16_t bootstrap_node_port, 
                bool generate_crypto_identity, const std::string& crypto_identity_path);

private slots:
    void update_chat_with_sent_message(std::shared_ptr<Message> message);
    void update_chat_with_received_message(std::shared_ptr<Message> message);
    void update_contacts_list_with_received_contact(std::shared_ptr<Contact> contact);

private slots:
    void on_logoutPushButton_clicked();
    void on_sendPushButton_clicked();
    void on_messageLineEdit_returnPressed();
    void on_searchLoginPushButton_clicked();
    void on_contactsListWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::ChatPage *ui_;
    std::shared_ptr<P2PMessengerImpl> p2p_messenger_impl_;
};

#endif // CHATPAGE_H
