#include "mytcpserver.h"

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr handle)
{
    qDebug() << "新客户端连接";
    //将连接的客户端socket存入socket列表
    MyTcpSocket* pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(handle);
    m_tcpSocketList.append(pTcpSocket);
    //连接移除socket的信号槽
    connect(pTcpSocket, &MyTcpSocket::offline, this, &MyTcpServer::deleteSocket);
}

void MyTcpServer::resend(char* strName, PDU *pdu)
{
    if(pdu == NULL || strName == NULL) return;
    for(int i = 0; i < m_tcpSocketList.size(); i++){
        if(m_tcpSocketList.at(i)->m_login_name == strName){
            qDebug() << "resend name:" << strName;
            m_tcpSocketList.at(i)->write((char*)pdu, pdu->uiPDULen);
            break;
        }
    }
}

void MyTcpServer::deleteSocket(MyTcpSocket* mysocket)
{
    m_tcpSocketList.removeOne(mysocket);
    mysocket->deleteLater();
    mysocket = NULL;

    //测试是否成功
    qDebug() << "m_tcpSocketList.size():" << m_tcpSocketList.size();
    for(int i = 0; i < m_tcpSocketList.size(); i++){
        qDebug() <<m_tcpSocketList.at(i)->m_login_name;
    }

}

MyTcpServer::MyTcpServer()
{
    
}
