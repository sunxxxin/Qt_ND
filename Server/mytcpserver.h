#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "mytcpsocket.h"

#include <QObject>
#include <QList>
#include <QTcpServer>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    static MyTcpServer& getInstance();
    //有客户端连接自动调用该函数
    void incomingConnection(qintptr handle);
    void resend(char* strName, PDU* pdu);
public slots:
    //移除下线的槽函数
    void deleteSocket(MyTcpSocket* mysocket);
private:
    MyTcpServer();

    MyTcpServer(const MyTcpServer& instance) = delete;
    MyTcpServer& operator = (const MyTcpServer&) = delete;
    //客户端socket指针列表
    QList<MyTcpSocket*> m_tcpSocketList;
};

#endif // MYTCPSERVER_H
