#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGlobal>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>

#ifdef Q_OS_WIN32
 #include "win_ui_mainwindow.h"
#else
 #include "ui_mainwindow.h"
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
  Q_OBJECT

  QNetworkAccessManager manager;

public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();
  bool saveToDisk(const QString &filename, QNetworkReply* reply);
  QString downloadLocation;
  QString FileName;

private:
  Ui::MainWindow *ui;
  QTimer *networkTimer;
  QString latestNetError;
  QString latestFileError;
  QNetworkReply *reply;

  void download(const QUrl &downTo,QNetworkReply *reply);
  QString getDownloadLocation(QString location);
  QString GetDefaultAppName(QString appName);
  QString GetDefaultAppLocation();

private slots:
  void start();
  void getlist();
  void getListFinished(QNetworkReply* reply);
  void downloadFinished(QNetworkReply *reply);
  void updateProgress(qint64 read, qint64 total);
  bool netHandleError(QNetworkReply* reply, QString urlDownload);
  void networkTimeout();
  static bool isHttpRedirect(QNetworkReply *reply);

};
#endif
