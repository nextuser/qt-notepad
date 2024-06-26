#include "mainwindow.h"

#include <QApplication>
#include <QPixmap>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString filePath;
    if(argc > 1){
        filePath = QString(argv[1]);
    }
    MainWindow w(nullptr,filePath);
    a.setWindowIcon(QIcon(":/images/images/icons8-notepad-32-blue.png"));
    w.show();
    return a.exec();
}
