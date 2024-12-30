#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QApplication::setWindowIcon(QIcon("C:/Users/theiv/Documents/Qt stuff/ck_3_history_character_creator/build/Desktop_Qt_6_8_1_MinGW_64_bit-Debug/applicationicon.png"));

    w.show();
    return a.exec();
}
