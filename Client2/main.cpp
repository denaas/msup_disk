#include "client2_window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client2_window w;
    w.show();

    return a.exec();
}
