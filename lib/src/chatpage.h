#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <memory>

#include <QDialog>

#include "p2p_messenger_impl.h"

namespace Ui {
class ChatPage;
}

class ChatPage : public QDialog {
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();

signals:
    void logged_in();
    void logged_out();
    void message_sent(const std::string& login, const std::string message);
    void message_received(const std::string& login, const std::string& message);
    void contact_received(const std::string& login, std::shared_ptr<dht::crypto::PublicKey> public_key, const std::string& address);

public slots:
    void log_in(const std::string& login, std::uint16_t dht_port, const std::string& ip,
                std::uint16_t port, bool generate_crypto_identity, const std::string& crypto_identity_path);

private slots:
    void update_chat_with_sent_message(const std::string& login, const std::string message);
    void update_chat_with_received_message(const std::string& login, const std::string& message);
    void update_contacts_list_with_received_contact(const std::string& login, std::shared_ptr<dht::crypto::PublicKey> public_key, const std::string& address);

private slots:
    void on_logoutPushButton_clicked();
    void on_sendPushButton_clicked();
    void on_messageLineEdit_returnPressed();
    void on_searchLoginPushButton_clicked();

private:
    Ui::ChatPage *ui_;
    std::shared_ptr<P2PMessengerImpl> p2p_messenger_impl_;
};

#endif // CHATPAGE_H
