#include "msghandler.h"
#include <stdlib.h>
#include "mytcpserver.h"
#include "operatedb.h"
#include "server.h"
#include <QDebug>
#include <QDir>

MsgHandler::MsgHandler()
{
    m_bUpload = false;
    m_bDownload = false;
}

PDU *MsgHandler::regist(PDU *pdu)
{
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    memcpy(caPwd, pdu->caData + 32, 32);
    qDebug() << "recvMsg REGIST caName:" << caName
             << "caPwd:" << caPwd;
    bool ret = OperateDB::getInstance().handleRegist(caName, caPwd);
    if(ret){
        QDir dir;
        dir.mkdir(QString("%1/%2").arg(Server::getInstance().getRootPath()).arg(caName));
    }
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPEND;
    memcpy(respdu->caData, &ret, sizeof (bool));
    return respdu;
}

PDU *MsgHandler::login(PDU *pdu, QString &m_login_name)
{
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    memcpy(caPwd, pdu->caData + 32, 32);
    qDebug() << "recvMsg LOGIN caName:" << caName
             << "caPwd:" << caPwd;
    m_login_name = caName;
    bool ret = OperateDB::getInstance().handleLogin(caName, caPwd);
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPEND;
    memcpy(respdu->caData, &ret, sizeof (bool));
    memcpy(respdu->caData + sizeof (bool), caName, 32);
    return respdu;
}

PDU *MsgHandler::findUser(PDU *pdu)
{
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    qDebug() << "recvMsg FINDUSER caName:" << caName;
    int ret = OperateDB::getInstance().handleFindUser(caName);
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_FINDUSER_RESPEND;
    memcpy(respdu->caData, &ret, sizeof(int));
    memcpy(respdu->caData + sizeof (int), &caName, 32);
    return respdu;
}

PDU *MsgHandler::onlineUser()
{
    QStringList onlineUserList = OperateDB::getInstance().handleOnlineUser();
    uint uiMsgLen = onlineUserList.size() * 32;
    PDU* respdu = mkPDU(uiMsgLen);
    respdu->uiMsgType = ENUM_MSG_TYPE_ONLINEUSER_RESPEND;
    for(int i = 0; i < onlineUserList.size(); i ++){
        memcpy(respdu->caMsg + i * 32, onlineUserList.at(i).toStdString().c_str(), 32);
        qDebug() << onlineUserList.at(i);
    }
    return respdu;
}

PDU *MsgHandler::addFriend(PDU *pdu)
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    memcpy(caTarName, pdu->caData + 32, 32);
    qDebug() << "recvMsg addFriend caCurName:" << caCurName
             << "caTarName:" << caTarName;
    int ret = OperateDB::getInstance().handleAddFriend(caCurName, caTarName);
    qDebug() << "handleAddFriend ret:" << ret;
    if(ret == 1){
        MyTcpServer::getInstance().resend(caTarName, pdu);
    }
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_ADDFRIEND_RESPEND;
    memcpy(respdu->caData, &ret, sizeof (int));
    return respdu;
}

PDU *MsgHandler::addFriendAgree(PDU *pdu)
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    memcpy(caTarName, pdu->caData + 32, 32);
    qDebug() << "recvMsg addFriendAgree caCurName:" << caCurName
             << "caTarName:" << caTarName;
    OperateDB::getInstance().handleAddFriendAgree(caCurName, caTarName);
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_ADDFRIEND_AGREE_RESPEND;
    MyTcpServer::getInstance().resend(caCurName, respdu);
    return respdu;
}

PDU *MsgHandler::flushOnlineFriend(PDU *pdu)
{
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    qDebug() << "recvMsg flushOnlineFriend caName:" << caName;
    QStringList res = OperateDB::getInstance().handleFlushOnlineFriend(caName);
    uint uiMsgLen = res.size() * 32;
    PDU* respdu = mkPDU(uiMsgLen);
    respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_ONLINE_FRIEND_RESPEND;
    for(int i = 0; i < res.size(); i ++){
        memcpy(respdu->caMsg + i * 32, res.at(i).toStdString().c_str(), 32);
        qDebug() << res.at(i);
    }
    return respdu;
}

PDU *MsgHandler::deleteFriend(PDU *pdu)
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    memcpy(caTarName, pdu->caData + 32, 32);
    qDebug() << "recvMsg addFriend caCurName:" << caCurName
             << "caTarName:" << caTarName;
    bool ret = OperateDB::getInstance().handleDeleteFriend(caCurName, caTarName);
    qDebug() << "handleDeleteFriend ret:" << ret;
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPEND;
    memcpy(respdu->caData, &ret, sizeof (bool));
    return respdu;
}

void MsgHandler::chat(PDU *pdu)
{
    char caTarName[32] = {'\0'};
    memcpy(caTarName, pdu->caData + 32, 32);
    MyTcpServer::getInstance().resend(caTarName, pdu);
}

PDU *MsgHandler::mkdir(PDU *pdu)
{
    QString strCurPath = pdu->caMsg;
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_MKDIR_RESPEND;
    bool res = false;
    QDir dir;
    if (!dir.exists(strCurPath)){
        memcpy(respdu->caData, &res, sizeof(bool));
        return respdu;
    }
    QString strNewPath = QString("%1/%2").arg(strCurPath).arg(pdu->caData);
    qDebug() << "mkdir strNewPath:" << strNewPath;
    if (dir.exists(strNewPath) || !dir.mkdir(strNewPath)){
        memcpy(respdu->caData, &res, sizeof(bool));
        return respdu;
    }
    res = true;
    memcpy(respdu->caData, &res, sizeof(bool));
    return respdu;
}

PDU *MsgHandler::flushFile(PDU *pdu)
{
    QString strPath = pdu->caMsg;
    QDir dir(strPath);
    QFileInfoList fileInfoList = dir.entryInfoList();
    int iFileCount = fileInfoList.size();

    PDU* respdu = mkPDU(sizeof(FileInfo) * (iFileCount - 2));
    respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPEND;
    FileInfo* pFileInfo = NULL;
    QString strFileName;
    for(int i = 0, j = 0; i < iFileCount; i++){
        strFileName = fileInfoList[i].fileName();
        if(strFileName == QString(".") ||strFileName == QString("..")){
            continue;
        }
        pFileInfo = (FileInfo*)(respdu->caMsg) + j++;
        memcpy(pFileInfo->caName, strFileName.toStdString().c_str(), 32);
        if(fileInfoList[i].isDir()){
            pFileInfo->iFileType = 0;//目录
        }else if(fileInfoList[i].isFile()){
            pFileInfo->iFileType = 1;//文件
        }
        qDebug() << "flushFile strFileName" << pFileInfo->caName
                 << "iFileType" << pFileInfo->iFileType;

    }
    return respdu;
}

PDU *MsgHandler::deleteDir(PDU *pdu)
{
    QFileInfo fileInfo(pdu->caMsg);
    bool ret = false;
    if(fileInfo.isDir()){
        QDir dir(pdu->caMsg);
        ret = dir.removeRecursively();
        qDebug() << "deleteDir ret:" << ret;
    }
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPEND;
    memcpy(respdu->caData, &ret, sizeof(bool));
    return respdu;
}

PDU *MsgHandler::deleteFile(PDU *pdu)
{
    QFileInfo fileInfo(pdu->caMsg);
    qDebug() << "deleteFile pdu->caMsg:" << pdu->caMsg;
    bool ret = false;
    if(fileInfo.isFile()){
        QDir dir;
        ret = dir.remove(pdu->caMsg);
        qDebug() << "deleteFile ret:" << ret;
    }
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPEND;
    memcpy(respdu->caData, &ret, sizeof(bool));
    return respdu;
}

PDU *MsgHandler::renameFile(PDU *pdu)
{
    //caData取原文件名和新文件名
    char caOldFileName[32] = {'\0'};
    char caNewFileName[32] = {'\0'};
    memcpy(caOldFileName, pdu->caData, 32);
    memcpy(caNewFileName, pdu->caData + 32, 32);
    //取当前路径拼接完整路径
    char* pPath = pdu->caMsg;

    QString strOldPath = QString("%1/%2").arg(pPath).arg(caOldFileName);
    QString strNewPath = QString("%1/%2").arg(pPath).arg(caNewFileName);
    //使用QDir对文件重命名
    QDir dir;
    bool ret = dir.rename(strOldPath, strNewPath);
    //结果发送给客户端
    qDebug() << "renameFile ret:" << ret;
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPEND;
    memcpy(respdu->caData, &ret, sizeof(bool));
    return respdu;
}

PDU *MsgHandler::moveFile(PDU *pdu)
{
    int srcLen = 0;
    int tarLen = 0;
    memcpy(&srcLen, pdu->caData, sizeof(int));
    memcpy(&tarLen, pdu->caData + sizeof(int), sizeof(int));

    char* pSrcPath = new char[srcLen + 1];
    char* pTarPath = new char[tarLen + 1];
    //清空申请的空间
    memset(pSrcPath, '\0', srcLen + 1);
    memset(pTarPath, '\0', tarLen + 1);

    memcpy(pSrcPath, pdu->caMsg, srcLen);
    memcpy(pTarPath, pdu->caMsg + srcLen, tarLen);

    bool ret = QFile::rename(pSrcPath, pTarPath);
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_RESPEND;
    memcpy(respdu->caData, &ret, sizeof(bool));
    delete [] pSrcPath;
    delete [] pTarPath;
    pSrcPath = NULL;
    pTarPath = NULL;
    return respdu;
}

PDU *MsgHandler::uploadFile(PDU *pdu)
{
    qDebug() << "uploadFile start";
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPEND;
    int ret = 0;
    if(m_bUpload){
        qDebug() << "m_bUpload is true";
        ret = 1;
        memcpy(respdu->caData, &ret, sizeof(int));
        return respdu;
    }
    char caFileName[32] = {'\0'};
    qint64 fileSize = 0;

    memcpy(caFileName, pdu->caData, 32);
    memcpy(&fileSize, pdu->caData + 32, sizeof(qint64));
    QString strPath = QString("%1/%2").arg(pdu->caMsg).arg(caFileName);
    m_fUploadFile.setFileName(strPath);
    if(!m_fUploadFile.open(QIODevice::WriteOnly)) {
        qDebug() << "file open failed";
        ret = -1;
        memcpy(respdu->caData, &ret, sizeof(int));
        return respdu;
    }
    if (fileSize == 0) {
        qDebug() << "file is empty";
        ret = 2;
        memcpy(respdu->caData, &ret, sizeof(int));
        return respdu;
    }
    qDebug() << "file open success";
    m_bUpload = true;
    m_iUploadTotal = fileSize;
    m_iUploadReceived = 0;
    memcpy(respdu->caData, &ret, sizeof(int));
    return respdu;
}

PDU *MsgHandler::uploadFileData(PDU *pdu)
{
    qDebug() << "-----------";
    m_fUploadFile.write(pdu->caMsg, pdu->uiMsgLen);
    m_iUploadReceived += pdu->uiMsgLen;
    if(m_iUploadReceived < m_iUploadTotal){
        qDebug() << "m_iUploadReceived < m_iUploadTotal";
        return NULL;
    }
    m_fUploadFile.close();
    m_bUpload = false;
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPEND;
    bool ret = m_iUploadReceived == m_iUploadTotal;
    qDebug() << "uploadFileData ret:" << ret;
    memcpy(respdu->caData, &ret, sizeof(bool));
    return respdu;
}

PDU *MsgHandler::downloadFile(PDU *pdu)
{
    qDebug() << "MsgHandler::downloadFile start";
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPEND;
    int ret = 0;
    if(m_bDownload){
        ret = 1;
        memcpy(respdu->caData, &ret, sizeof(int));
        return respdu;
    }
    QFileInfo fileInfo(pdu->caMsg);
    qint64 fileSize = fileInfo.size();
    m_fDownloadFile.setFileName(pdu->caMsg);
    qDebug() << "downloadFile pdu->caMsg:" << pdu->caMsg;
    if(m_fDownloadFile.open(QIODevice::ReadOnly)){
        m_bDownload = true;
        qDebug() << "m_bDownload = true";
    }else{
        ret = -1;
    }
    qDebug() << "downloadFile ret:" << ret;
    memcpy(respdu->caData, &ret, sizeof(int));
    memcpy(respdu->caData + sizeof(int), &fileSize, sizeof(qint64));
    qDebug() << "MsgHandler::downloadFile end";
    return respdu;
}

PDU *MsgHandler::downloadFileData(QTcpSocket *socket)
{
    qDebug() << "MsgHandler::downloadFileData start";
    PDU* respdu = mkPDU(4096);
    respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA_RESPEND;
    qint64 ret = 0;
    while(true){
        ret = m_fDownloadFile.read(respdu->caMsg, 4096);
        if(ret <= 0){
            break;
        }
        respdu->uiMsgLen = ret;
        respdu->uiPDULen = ret + sizeof(PDU);
        socket->write((char*)respdu, respdu->uiPDULen);
    }
    m_bDownload = false;
    m_fDownloadFile.close();
    free(respdu);
    respdu = NULL;
    qDebug() << "MsgHandler::downloadFileData end";
    return NULL;
}

PDU *MsgHandler::shareFile(PDU *pdu)
{
    char caCurName[32] = {'\0'};
    int friendNum = 0;
    memcpy(caCurName, pdu->caData, 32);
    memcpy(&friendNum, pdu->caData + 32, sizeof(int));

    int size = friendNum * 32;

    PDU* resendpdu = mkPDU(pdu->uiMsgLen - size);
    resendpdu->uiMsgType = pdu->uiMsgType;

    memcpy(resendpdu->caData, caCurName, 32);
    memcpy(resendpdu->caMsg, pdu->caMsg + size, pdu->uiMsgLen - size);

    char caRecvName[32] = {'\0'};
    for(int i = 0; i < friendNum; i++){
        memcpy(caRecvName, pdu->caMsg + i * 32, 32);
        MyTcpServer::getInstance().resend(caRecvName, resendpdu);
    }
    free(resendpdu);
    resendpdu = NULL;

    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPEND;
    return respdu;
}

PDU *MsgHandler::shareFileAgree(PDU *pdu)
{
    QString strSharePath = pdu->caMsg;
    QString strRecvPath = QString("%1/%2").arg(Server::getInstance().getRootPath()).arg(pdu->caData);
    qDebug() << "strSharePath:" << strSharePath;
    qDebug() << "strRecvPath:" << strRecvPath;
    int index = strSharePath.lastIndexOf('/');
    QString strFileName = strSharePath.right(strSharePath.size() - index - 1);
    strRecvPath = strRecvPath + "/" + strFileName;
    qDebug() << "strSharePath:" << strSharePath;
    qDebug() << "strRecvPath:" << strRecvPath;
    bool ret = QFile::copy(strSharePath, strRecvPath);

    PDU* respdu = mkPDU(0);
    memcpy(respdu->caData, &ret, sizeof(bool));
    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPEND;
    return respdu;
}
