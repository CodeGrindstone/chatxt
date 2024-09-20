#include "mainwindow.h"
#include "global.h"
#include <QFile>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //解析config文件
    QString app_path = QCoreApplication::applicationDirPath();
    QDir dir_apppath(app_path);
    dir_apppath.cdUp();
    dir_apppath.cdUp();
    qDebug() << dir_apppath.absolutePath();
    QString fileName = "config.ini";
    QString config_path = dir_apppath.absolutePath()+ QDir::separator() + fileName;
    qDebug() << config_path;
    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://"+gate_host+":"+gate_port;

    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly)){
        qDebug("open success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else{
        qDebug("Open failed");
    }

    MainWindow w;
    w.show();
    return a.exec();
}


