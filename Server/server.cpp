#include "server.h"
#include "ui_server.h"
#include <QFile>
#include <QDebug>
#include <QHostAddress>
#include "mytcpserver.h"
Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    //加载配置
    loadConfig();
    //监听
    MyTcpServer::getInstance().listen(QHostAddress(m_strIp), m_usPort);

}

Server::~Server()
{
    delete ui;
}

void Server::loadConfig()
{
    QFile file(":/server.config");//固定冒号开头
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = QString(baData);
        QStringList strList = strData.split("\r\n");
        m_strIp = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        m_strRootPath = strList.at(2);
        qDebug() << "打开服务器配置文件ip:" << m_strIp << "port:" << m_usPort << "RootPath:" << m_strRootPath;
        file.close();
    }else{
        qDebug() << "打开配置失败";
    }
}

Server &Server::getInstance()
{
    static Server instance;
    return instance;
}

QString Server::getRootPath()
{
    return m_strRootPath;
}

