#include "p2pmessenger.h"

#include <memory>

#include <QApplication>
#include <QMetaType>

#include "contact.h"
#include "message.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<std::shared_ptr<Contact>>("std::shared_ptr<Contact>");
    qRegisterMetaType<std::shared_ptr<Message>>("std::shared_ptr<Message>");
    P2PMessenger w;
    w.show();
    return a.exec();
}
