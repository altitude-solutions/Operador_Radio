#include "login.h"

#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login w;

    QIcon icon(":/images/LPL.ico");
    w.setWindowIcon(icon);

    w.show();
    return a.exec();
}
