#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "protocol.h"

#include <QString>



class ResHandler
{
public:
    ResHandler();
    void regist(PDU *pdu);
    void login(PDU *pdu, QString& m_login_name);
    void findUser(PDU *pdu);
    void onlineUser(PDU *pdu, QString &m_login_name);
    void addFriend(PDU* pdu);
    void addFriendRequest(PDU* pdu);
    void addFriendAgree(PDU* pdu);
    void flushOnlineFriend(PDU* pdu);
    void deleteFriend(PDU* pdu);
    void chat(PDU* pdu);
    void mkdir(PDU* pdu);
    void flushFile(PDU* pdu);
    void deleteDir(PDU* pdu);
    void deleteFile(PDU* pdu);
    void renameFile(PDU* pdu);
    void moveFile(PDU* pdu);
    void uploadFile(PDU* pdu);
    void uploadFileData(PDU* pdu);
    void downloadFile(PDU* pdu);
    void downloadFileData(PDU* pdu);
    void shareFile(PDU* pdu);
    void shareFileRequest(PDU* pdu);
    void shareFileAgree(PDU* pdu);

};

#endif // MSGHANDLER_H
