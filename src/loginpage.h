#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QDialog>

namespace Ui {
class LoginPage;
}

class LoginPage : public QDialog {
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();

signals:
    void log_in_requsted(const std::string& login, std::uint16_t dht_port, const std::string& ip,
                         std::uint16_t port, bool generate_crypto_identity, const std::string& crypto_identity_path);

private slots:
    void on_loginPushButton_clicked();

private:
    Ui::LoginPage *ui_;
};

#endif // LOGINPAGE_H
