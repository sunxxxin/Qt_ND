#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include "protocol.h"
#include "msghandler.h"
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString m_login_name;
//    PDU* readPDU();
    PDU* handlePDU(PDU* pdu);
    MsgHandler* mh;
    ~MyTcpSocket();
    void sendPDU(PDU* pdu);
    QByteArray buffer;
public slots:
    void recvMsg();//接收消息的处理函数
    void clientOffline();
signals:
    void offline(MyTcpSocket* mysocket);


};

#endif // MYTCPSOCKET_H
