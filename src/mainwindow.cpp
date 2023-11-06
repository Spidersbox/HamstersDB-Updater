#include <QtGlobal> // for checking qt version
#include<QDebug> // for qDebug()
#include <QSslSocket>

#if QT_VERSION >= 0x050000
 #include <QApplication>
 #include <QtWidgets>
#else
 #include <QtGui>
 #include <QtGui/QApplication>
#endif

#include <QUrl>

#ifdef Q_OS_MAC
#include "macdockiconhandler.h"
#endif

#ifdef Q_OS_WIN32
 #include <shlobj.h>
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "version.h"

bool sucess=false;

QString icon="icon";// name of your icon in the res file
QString dirName="HamstersDB";
QString appName="HamstersDB";
QString downLocation="http://down.republichams.net/Downloads/"+dirName;

//QString appPath=QDir::homePath()+"/"+dirName+"/";

//-------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  //default center of screen
  int x,y;
#if QT_VERSION < 0x060000
  QRect desktopRect = QApplication::desktop()->availableGeometry(this);
  QPoint center = desktopRect.center();
  x=center.x() - width() * 0.5;
  y=center.y() - height() * 0.5;
#else
  QScreen *screen = QGuiApplication::primaryScreen();
  QRect  screenGeometry = screen->geometry();
  x=screenGeometry.height()/2 -225;
  y=screenGeometry.width()/2 -400;
#endif

  resize(280,280);
  move(x,y);

  setWindowTitle(appName+" Launcher-Updater "+CLIENT_BUILD);

  networkTimer = new QTimer(this);
  networkTimer->setInterval(10000);
  connect(networkTimer, SIGNAL(timeout()), this, SLOT(networkTimeout()));

  ui->TextEdit->append("working ...");

#ifndef Q_OS_MAC
    qApp->setWindowIcon(QIcon(":images/"+icon));
    setWindowIcon(QIcon(":images/"+icon));
#else
//    setUnifiedTitleAndToolBarOnMac(true);
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

// give some time for the form to show up
  QTimer::singleShot(1000, this, SLOT(start()));
}

//--------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
  delete ui;
}

//-------------------------------------------------------------------------------------
void MainWindow::start()
{
//qDebug()<<"Is SSL Enabled? "<<QSslSocket::supportsSsl();
//qDebug()<<"SSL Library Build Version " << QSslSocket::sslLibraryBuildVersionString();
//qDebug()<<"SSL Library Version String (locally) "<<QSslSocket::sslLibraryVersionString();

  QString appPath=GetDefaultAppLocation();
  QString temp="checking path "+appPath;
qDebug()<<temp<<"\n";
  ui->TextEdit->append(temp+"\n");
  try
  {
    // look for app dir
    QDir dir(appPath);
    if (!dir.exists())
    {
      temp="creating "+appPath;
qDebug()<<temp;
      ui->TextEdit->append(temp+"\n");
      if(!dir.mkpath(appPath))
      {
        temp="can not create "+appPath;
qDebug()<<temp;
        ui->TextEdit->append(temp);
        QMessageBox::information(this,"error","can not create "+appPath);
      }
    }
    else
    {
      temp="path exists.";
qDebug()<<temp;
      ui->TextEdit->append(temp);
    }
  }
  catch (std::exception& e)
  {
    QMessageBox::information(this,"creating path",e.what());
  }

// get local version.txt and read it
  QString filename = appPath+"version.txt";
  QFile file(filename);
  if(!file.open(QIODevice::ReadOnly))
  {
    if(file.error() !=5) // no such file or directory
    {
      QMessageBox::information(0, "error", file.errorString());
    }
  }
  QTextStream in(&file);

  QString localtx = in.readLine();
  file.close();
  if(!localtx.length())
  {
    localtx="0000";
  }

  ui->label3->setText(localtx);

  // get remote version.txt and read it
  getlist();
  while(networkTimer->isActive())
  {// wait for download to finish
    qApp->processEvents();
  }

  int iLocalVer=ui->label3->text().toInt();
  int iRemoteVer=ui->label4->text().toInt();
  if(iRemoteVer > iLocalVer)
  {
    temp="Found new version";
    ui->TextEdit->append(temp);
qDebug()<<temp;

    download(appPath,NULL);
  }  
  else
  { // launch app
    temp="version is current";
    ui->TextEdit->append(temp);
qDebug()<<temp;
    temp="launching";
    ui->TextEdit->append(temp);
    QString strAppFilename=GetDefaultAppName(appName);
//qDebug()<<strAppFilename;
    QString strPathApp="\""+appPath+strAppFilename+"\"";
//qDebug()<<"launching "<<strPathApp;
    QProcess::startDetached(strPathApp);
    exit(0);
  }
}

//-------------------------------------------------------------------------------------
void MainWindow::getlist()
{
  // get remote version
  // connect the event and launch it
  connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getListFinished(QNetworkReply*)));

  QNetworkRequest request;
  downloadLocation=getDownloadLocation(downLocation)+"version.txt";

  request.setUrl(QUrl(downloadLocation));
  request.setRawHeader("User-Agent", "HamstersDB update request");

  QSslConfiguration conf = request.sslConfiguration();
  conf.setPeerVerifyMode(QSslSocket::VerifyNone);
  request.setSslConfiguration(conf);

  networkTimer->start();
  manager.get(request);
}

//-------------------------------------------------------------------------------------
void MainWindow::getListFinished(QNetworkReply* reply)
{
  if (netHandleError(reply, downloadLocation))
  {
    disconnect(&manager, SIGNAL(finished(QNetworkReply*)), 0, 0);  
    connect(&manager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
    QString versionlist=reply->readAll();

    QString line=versionlist;
    ui->label4->setText(line);
  }
  else
  {
    reply->abort();
  } 
}

//-------------------------------------------------------------------------------------
void MainWindow::download(const QUrl &downTo,QNetworkReply *reply)
{
  networkTimer->stop();
  QString temp=downTo.toString();
  QString appFilename=GetDefaultAppName(appName);

// get app
  disconnect(&manager, SIGNAL(finished(QNetworkReply*)), 0, 0);  
  connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
  QNetworkRequest request;

  QString tempurl=getDownloadLocation(downLocation)+"/"+appFilename;
//qDebug()<<"tempurl "<<tempurl;
  request.setUrl(tempurl);
  request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
  QString mess="requesting "+tempurl;
  ui->TextEdit->append(mess+"\n");

  request.setRawHeader("User-Agent", "app request");
  FileName=temp+appFilename;

  networkTimer->start();
  reply = manager.get(request);
  connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(updateProgress(qint64, qint64)));
  while(networkTimer->isActive())
  {// wait for download to finish
    qApp->processEvents();
  }
  QFile myfile(FileName);
  myfile.setPermissions(QFile::ReadOwner|QFile::WriteOwner|QFile::ExeOwner|QFile::ReadGroup| QFile::ExeGroup|QFile::ReadOther|QFile::ExeOther);

// get version.txt
  disconnect(&manager, SIGNAL(finished(QNetworkReply*)), 0, 0);  
  connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
  request.setUrl(downloadLocation);
  request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
  request.setRawHeader("User-Agent", "version.txt request");

  QString appPath=GetDefaultAppLocation();
  FileName=appPath+"version.txt";
  networkTimer->start();
  reply = manager.get(request);
  connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(updateProgress(qint64, qint64)));

  while(networkTimer->isActive())
  {// wait for download to finish
    qApp->processEvents();
  }

  // launch app
  QString strAppFilename=GetDefaultAppName(appName);
  QString strPathApp=appPath+strAppFilename;
qDebug()<<"launching "<<strPathApp;
  mess="launching  "+strAppFilename;
  ui->TextEdit->append(mess);
  QProcess::startDetached(strPathApp);
  exit(0);
}

//-------------------------------------------------------------------------------------
void MainWindow::downloadFinished(QNetworkReply *reply)
{
  networkTimer->stop();
  QString mess;

  if (!saveToDisk(FileName, reply))
  {
    QString fError="Could not open "+FileName+" for writing: "+latestFileError;
    mess="File Saving Error"+fError;
    ui->TextEdit->append(mess);
  }

  disconnect(&manager, SIGNAL(finished(QNetworkReply*)), 0, 0);  
  mess="downloadFinished ...\n";
  ui->TextEdit->append(mess);
}

//-------------------------------------------------------------------------------------
bool MainWindow::saveToDisk(const QString &filename, QNetworkReply *reply)
{
  QString mess="saving "+filename;
  ui->TextEdit->append(mess);
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly))
  {
    latestFileError = file.errorString();
    mess="file open error "+file.errorString();
    ui->TextEdit->append(mess);

    return false;
  }

  file.write(reply->readAll());
  file.close();

  ui->progressBar->setValue(0);
  return true;
}

//-------------------------------------------------------------------------------------
void MainWindow::updateProgress(qint64 read, qint64 total)
{
  ui->progressBar->setMaximum(total);
  ui->progressBar->setValue(read);
  networkTimer->start(); // restart the timer so it doesn't timeout on long downloads
}

//-------------------------------------------------------------------------------------
bool MainWindow::netHandleError(QNetworkReply* reply, QString urlDownload)
{
  networkTimer->stop();
  if (reply->error())
  {
    latestNetError = tr("Download of ") + urlDownload + tr(" failed: ") + reply->errorString();
  }
  else if (isHttpRedirect(reply))
  {
    latestNetError = tr("HTTP redirect while attempting download of ") + urlDownload;
  }
  else
 {
  // signal no errors here
  return(true);
  }

  // execute the same function, displaying latest occured error
  QString mess="network timeout.";
  ui->TextEdit->append(mess);
  networkTimeout();
  return(false);
}

//-------------------------------------------------------------------------------------
void MainWindow::networkTimeout()
{
  // signal error and preset for next operation
  networkTimer->stop();
  if (latestNetError == "")
  {
    latestNetError ="Network timeout. Please check your network and try again.";
  }
  disconnect(&manager, SIGNAL(finished(QNetworkReply*)), 0, 0);  
  QMessageBox::information(this,"Network Error",latestNetError);
}

//-------------------------------------------------------------------------------------
bool MainWindow::isHttpRedirect(QNetworkReply *reply)
{
  int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  return statusCode == 301 || statusCode == 302 || statusCode == 303
      || statusCode == 305 || statusCode == 307 || statusCode == 308;
}

//-------------------------------------------------------------------------------------
QString MainWindow::getDownloadLocation(QString location)
{
#ifdef WIN32
  // Windows
  return location+"/win/";
#endif

#ifdef MAC_OSX
  // MAC
  return location+"/mac/";
#endif

  // Linux
  return location+"/linux/";
}

//-------------------------------------------------------------------------------------
QString MainWindow::GetDefaultAppName(QString appName)
{
#ifdef WIN32
  // Windows
  return appName+".exe";
#endif

#ifdef MAC_OSX
  // MAC
  return appName;
#endif

  // Unix
  return appName;
}

//-------------------------------------------------------------------------------------
QString MainWindow::GetDefaultAppLocation()
{
#ifdef WIN32
  // Windows
//  return "c:/Program Files/"+dirName+"/";
return QDir::homePath()+"/AppData/Roaming/"+dirName+"/";
#endif

#ifdef MAC_OSX
  // MAC
  return "/Applications/"+dirName+".app/Contents/MacOS/";
#endif

  // Unix
  return QDir::homePath()+dirName+"/";
}
