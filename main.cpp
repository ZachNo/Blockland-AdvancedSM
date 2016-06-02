#include "mainwindow.h"
#include "currentPlayers.h"
#include <QApplication>

//Normal initialization of Qt program
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont Font("Arial", 8);
    a.setFont(Font);
    MainWindow mWindow;
    mWindow.setApp(&a);
    mWindow.show();

    return a.exec();
}
