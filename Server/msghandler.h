#ifndef MSGHANDLER_H
#define MSGHANDLER_H
//#include "mytcpsocket.h"
#include "protocol.h"
#include <QFile>
#include<QString>
#include<QTcpSocket>
class MsgHandler
{
public:
    MsgHandler();
    PDU* regist(PDU* pdu);
    PDU* login(PDU* pdu, QString &m_login_name);
    PDU* findUser(PDU* pdu);
    PDU* onlineUser();
    PDU* addFriend(PDU* pdu);
    PDU* addFriendAgree(PDU* pdu);
    PDU* flushOnlineFriend(PDU* pdu);
    PDU* deleteFriend(PDU* pdu);
    void chat(PDU* pdu);
    PDU* mkdir(PDU* pdu);
    PDU* flushFile(PDU* pdu);
    PDU* deleteDir(PDU* pdu);
    PDU* deleteFile(PDU* pdu);
    PDU* renameFile(PDU* pdu);
    PDU* moveFile(PDU* pdu);
    PDU* uploadFile(PDU* pdu);
    PDU* uploadFileData(PDU* pdu);
    PDU* downloadFile(PDU* pdu);
    PDU* downloadFileData(QTcpSocket *socket);
    PDU* shareFile(PDU* pdu);
     PDU* shareFileAgree(PDU* pdu);

    //上传文件对象
    QFile m_fUploadFile;
    //上传文件夹大小
    qint64 m_iUploadTotal;
    //以上传大小
    qint64 m_iUploadReceived;
    //是否在上传
    bool m_bUpload;



    QFile m_fDownloadFile;
    bool m_bDownload;


};

#endif // MSGHANDLER_H
