#include "p2p_messenger.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    P2PMessenger w;
    w.show();
    return a.exec();
}
