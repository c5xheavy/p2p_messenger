#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <memory>

#include <QDialog>

#include "p2p_messenger_impl.h"

namespace Ui {
class ChatPage;
}

class ChatPage : public QDialog
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();

signals:
    void on_AfterSuccessfulLogin_ChatPage();
    void on_Logout_ChatPage();

public slots:
    void on_SuccessfulLogin_ChatPage(const std::string& login, std::uint16_t dht_port, const std::string& ip, std::uint16_t port);

private slots:
    void on_logoutPushButton_clicked();

    void on_sendPushButton_clicked();

    void on_messageLineEdit_returnPressed();

private:
    Ui::ChatPage *ui;

    std::shared_ptr<P2PMessengerImpl> p2p_messenger_impl_;
};

#endif // CHATPAGE_H
