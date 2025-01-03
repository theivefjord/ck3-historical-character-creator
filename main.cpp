#include "mainwindow.h"
#include "windowframe.h"

#include <QApplication>
#include <QFile>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // force fusion for consistency
    a.setStyle(QStyleFactory::create("Fusion"));
    // set to my sick style sheet that I worked on for too long (fuck css/qss 🤢)
    QFile file(":/stylesheet/assets/grayboi.qss");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        a.setStyleSheet(file.readAll());
        file.close();
    }
    // passing a windowframe pointer to the mainwindow class so mainwindow has a reference to the windowframe
    // i need this so I can rename the window using the function in windowframe.cpp
    // i think this is called but dependency injection but i'm not sure if it's the best way to do it
    // this became necessary after removing the window native frame and building my custom title bar in a different class
    MainWindow *mw = new MainWindow;
    WindowFrame w(nullptr, mw);
    mw->setWindowFrame(&w);

    w.show();
    return a.exec();
}
