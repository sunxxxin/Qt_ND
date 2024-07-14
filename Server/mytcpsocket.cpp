#include "mytcpsocket.h"
#include "operatedb.h"
#include "protocol.h"
#include <QDebug>
MyTcpSocket::MyTcpSocket()
{
    mh = new MsgHandler;
    connect(this, &QTcpSocket::readyRead, this, &MyTcpSocket::recvMsg);
    connect(this, &QTcpSocket::disconnected, this, &MyTcpSocket::clientOffline);
}

//PDU *MyTcpSocket::readPDU()
//{



//    //读取协议总长度uiPDULen
//    uint uiPDULen = 0;
//    this->read((char*)&uiPDULen, sizeof(uint));

//    uint uiMsgLen = uiPDULen - sizeof(PDU);
//    PDU* pdu = mkPDU(uiMsgLen);
//    //读取除了协议总长度以外的内容
//    this->read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));
//    qDebug() << "recvMsg消息类型：" << pdu->uiMsgType
//             << "参数内容1：" << pdu->caData
//             << "参数内容2：" << pdu->caData + 32
//             << "消息内容：" << pdu->caMsg;
//    return pdu;
//}

PDU *MyTcpSocket::handlePDU(PDU *pdu)
{
    qDebug() << "recvMsg消息类型：" << pdu->uiMsgType
             << "参数内容1：" << pdu->caData
             << "参数内容2：" << pdu->caData + 32
             << "消息内容：" << pdu->caMsg;
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_REQUEST:
        return mh->regist(pdu);
    case ENUM_MSG_TYPE_LOGIN_REQUEST:
        return mh->login(pdu,m_login_name);
    case ENUM_MSG_TYPE_FINDUSER_REQUEST:
        return mh->findUser(pdu);
    case ENUM_MSG_TYPE_ONLINEUSER_REQUEST:
        return mh->onlineUser();
    case ENUM_MSG_TYPE_ADDFRIEND_REQUEST:
        return mh->addFriend(pdu);
    case ENUM_MSG_TYPE_ADDFRIEND_AGREE_REQUEST:
        return mh->addFriendAgree(pdu);
    case ENUM_MSG_TYPE_FLUSH_ONLINE_FRIEND_REQUEST:
        return mh->flushOnlineFriend(pdu);
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
        return mh->deleteFriend(pdu);
    case ENUM_MSG_TYPE_CHAT_REQUEST:
        mh->chat(pdu);
        break;
    case ENUM_MSG_TYPE_MKDIR_REQUEST:
        return mh->mkdir(pdu);
    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
        return mh->flushFile(pdu);
    case ENUM_MSG_TYPE_DEL_DIR_REQUEST:
        return mh->deleteDir(pdu);
    case ENUM_MSG_TYPE_DEL_FILE_REQUEST:
        return mh->deleteFile(pdu);
    case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:
        return mh->renameFile(pdu);
    case ENUM_MSG_TYPE_MOVE_FILE_REQUEST:
        return mh->moveFile(pdu);
    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
        return mh->uploadFile(pdu);
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST:
        return mh->uploadFileData(pdu);
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:
        return mh->downloadFile(pdu);
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA_REQUEST:
        return mh->downloadFileData(this);
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
        return mh->shareFile(pdu);
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST:
        return mh->shareFileAgree(pdu);
    default:
        break;
    }
    return NULL;
}

MyTcpSocket::~MyTcpSocket()
{
    delete mh;
}

void MyTcpSocket::sendPDU(PDU *respdu)
{
    if(respdu == NULL) return;
    write((char*)respdu, respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void MyTcpSocket::recvMsg()
{
    qDebug() << "\n\n\nrecvMsg 接收的消息长度" << this->bytesAvailable();
    QByteArray data = readAll();
    buffer.append(data);
    qDebug() << "buffer.size() : " << buffer.size() << "sizeof(PDU)" << sizeof(PDU);
    while(buffer.size() >= int(sizeof(PDU))){//判断是否够一个完整的PDU，为了能取PDU中的协议长度
        PDU* pdu = (PDU*)buffer.data();
        if(buffer.size() < int(pdu->uiPDULen)){//判断是否够一个完整的协议请求

            break;
        }

        PDU* respdu = handlePDU(pdu);
        sendPDU(respdu);
        buffer.remove(0, pdu->uiPDULen);
    }

}

void MyTcpSocket::clientOffline()
{
    OperateDB::getInstance().handOffline(m_login_name.toStdString().c_str());
    emit offline(this);
}
