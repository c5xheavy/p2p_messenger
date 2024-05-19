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

private slots:
    void switch_to_chat_page();
    void switch_to_login_page();

private:
    Ui::P2PMessenger *ui_;
};

#endif // P2PMESSENGER_H
