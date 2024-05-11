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
    void LogIn();
    void LogOut();
    void UpdateChatWithSentMessage(const std::string& login, const std::string message);
    void UpdateChatWithReceivedMessage(const std::string& login, const std::string& message);
    void UpdateContactsList(const std::string& login, const std::string& address);

public slots:
    void ReceivedLoginParameters(const std::string& login, std::uint16_t dht_port, const std::string& ip, std::uint16_t port);

private slots:
    void UpdatedChatWithSentMessage(const std::string& login, const std::string message);
    void UpdatedChatWithReceivedMessage(const std::string& login, const std::string& message);
    void UpdatedContactsList(const std::string& login, const std::string& address);

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
