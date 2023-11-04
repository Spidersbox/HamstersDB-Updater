#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
//  a.setWindowIcon(QIcon("./images/ant-icon1.ico"));
//  a.setWindowIcon(QIcon(":/images/antenna"));

  MainWindow w;
  w.show();
  return a.exec();
}

