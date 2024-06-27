#include "tsubwindow.h"
#include <QMenu>
#include "mainwindow.h"
TSubWindow::TSubWindow(QWidget *parent,MainWindow * window):QMdiSubWindow(parent),mainWindow(window)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ///setContextMenuPolicy(Qt::CustomContextMenu);
    setSystemMenu(window->createTitleMenu(this));
}

