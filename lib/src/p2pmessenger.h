#ifndef P2PMESSENGER_H
#define P2PMESSENGER_H

#include <QMainWindow>

#include <cstdint>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class P2PMessenger; }
QT_END_NAMESPACE

class P2PMessenger : public QMainWindow {
    Q_OBJECT

public:
    P2PMessenger(QWidget *parent = nullptr);
    ~P2PMessenger();

signals:
    void SendLoginParameters(const std::string& login, std::uint16_t dht_port, const std::string& ip, std::uint16_t port);

private slots:
    void ReceivedLoginParameters(const std::string& login, std::uint16_t dht_port, const std::string& ip, std::uint16_t port);
    void on_AfterSuccessfulLogin_P2PMessenger();
    void on_Logout_P2PMessenger();

private:
    Ui::P2PMessenger *ui;
};

#endif // P2PMESSENGER_H
