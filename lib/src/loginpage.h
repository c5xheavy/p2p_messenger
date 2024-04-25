#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QDialog>

namespace Ui {
class LoginPage;
}

class LoginPage : public QDialog
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();

signals:
    void on_Login_LoginPage(const std::string& login, std::uint16_t dht_port, const std::string& ip, std::uint16_t port);

private slots:
    void on_loginPushButton_clicked();

private:
    Ui::LoginPage *ui;
};

#endif // LOGINPAGE_H