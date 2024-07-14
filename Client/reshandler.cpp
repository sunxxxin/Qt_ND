#include "client.h"
#include "index.h"
#include "reshandler.h"
#include <QMessageBox>
ResHandler::ResHandler()
{

}
void ResHandler::regist(PDU *pdu){
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(ret){
        QMessageBox::information(&Client::getInstance(), "注册", "注册成功");
    }else{
        QMessageBox::information(&Client::getInstance(), "注册", "注册失败");
    }
}

void ResHandler::login(PDU *pdu, QString& m_login_name)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    char MLname[32] = {'\0'};
    qDebug() << "bool ret1: " << ret;
    memcpy(MLname, pdu->caData + sizeof (bool), 32);
    qDebug() << "bool ret2: " << ret;
    m_login_name = MLname;
    qDebug() << "m_login_name: " << m_login_name;

    if(ret){
        QMessageBox::information(&Client::getInstance(), "登录", "登录成功");
        Index::getInstance().show();
        Index::getInstance().setWindowTitle(Client::getInstance().m_login_name);
        Client::getInstance().hide();
    }else{
        QMessageBox::information(&Client::getInstance(), "登录", "登录失败，用户或密码错误");
    }
}

void ResHandler::findUser(PDU *pdu)
{
    int ret;
    char caName[32] = {'\0'};
    memcpy(&ret, pdu->caData, sizeof(int));
    memcpy(caName, pdu->caData + sizeof(int), 32);
    if(ret == -1){
        QMessageBox::information(&Index::getInstance(), "查找", QString("用户'%1'不存在").arg(caName));
    }else if(ret == 0){
        QMessageBox::information(&Index::getInstance(), "查找", QString("用户'%1'离线").arg(caName));
    }else if(ret == 1){
        int ret = QMessageBox::information(&Index::getInstance(), "查找", QString("用户'%1'在线").arg(caName), "添加好友", "取消");
        if(ret != 0){
            return;
        }
        PDU* pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ADDFRIEND_REQUEST;
        memcpy(pdu->caData, Client::getInstance().m_login_name.toStdString().c_str(), 32);
        memcpy(pdu->caData + 32, caName, 32);
        Client::getInstance().sendPDU(pdu);
    }
}

void ResHandler::onlineUser(PDU *pdu, QString &m_login_name)
{
    uint uiSize = pdu->uiMsgLen / 32;
    QStringList userList;
    char caTmp[32] = {'\0'};
    for(uint i = 0; i < uiSize; i++){
        memcpy(caTmp, pdu->caMsg + i * 32, 32);
        if(QString(caTmp) == m_login_name){
            continue;
        }
        userList.append(QString(caTmp));
    }
    Index::getInstance().getFriend()->get_m_pOnlineUser()->uodateOnlineUserWigetList(userList);
}

void ResHandler::addFriend(PDU *pdu)
{
    int ret;
    memcpy(&ret, pdu->caData, sizeof(int));
    if(ret == -1){
        QMessageBox::information(&Index::getInstance(), "添加好友", "添加错误、服务器内部错误");
    }else if(ret == -2){
        QMessageBox::information(&Index::getInstance(), "添加好友", "对方已经是你的好友");
    }else if(ret == 0){
        QMessageBox::information(&Index::getInstance(), "添加好友", "对方不在线");
    }else{
        return;
    }
}

void ResHandler::addFriendRequest(PDU *pdu)
{
    char caName[32] = {"\0"};
    memcpy(caName, pdu->caData,32);
    int ret = QMessageBox::question(&Index::getInstance(),"添加好友请求", QString("是否同意%1的添加好友请求").arg(caName));
    if(ret != QMessageBox::Yes){
        return;
    }
    PDU* respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_ADDFRIEND_AGREE_REQUEST;
    memcpy(respdu->caData, pdu->caData, 64);
    Client::getInstance().sendPDU(respdu);
}

void ResHandler::addFriendAgree(PDU *pdu)
{
    QMessageBox::information(&Index::getInstance(), "添加好友", "添加成功");
    Index::getInstance().getFriend()->flushFriend();
}

void ResHandler::flushOnlineFriend(PDU *pdu)
{
    uint uiSize = pdu->uiMsgLen / 32;
    QStringList userList;
    char caTmp[32] = {'\0'};
    for(uint i = 0; i < uiSize; i++){
        memcpy(caTmp, pdu->caMsg + i * 32, 32);
        userList.append(QString(caTmp));
    }
    Index::getInstance().getFriend()->updateFriendLW(userList);
}

void ResHandler::deleteFriend(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));

    if(ret){
        QMessageBox::information(Index::getInstance().getFriend(), "删除好友", "删除成功");
        Client::getInstance().hide();
    }else{
        QMessageBox::information(Index::getInstance().getFriend(), "删除好友", "删除失败，对方已不是好友");
    }
    Index::getInstance().getFriend()->flushFriend();
}

void ResHandler::chat(PDU *pdu)
{
    Chat* c = Index::getInstance().getFriend()->getChat();
    if(c->isHidden()){
        c->show();
    }
    char caChatName[32] = {'\0'};
    memcpy(caChatName, pdu->caData, 32);
    QString strMsg = QString("%1: %2").arg(caChatName).arg(pdu->caMsg);
    c->m_strChatName = caChatName;
    c->updateShow_LE(strMsg);
}

void ResHandler::mkdir(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));

    if(ret){
        QMessageBox::information(Index::getInstance().getFile(), "创建文件夹", "创建成功");
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(Index::getInstance().getFile(), "创建文件夹", "创建失败");
    }
}

void ResHandler::flushFile(PDU *pdu)
{
    int iCount = pdu->uiMsgLen / sizeof(FileInfo);

    QList<FileInfo*> pFileList;

    for(int i = 0; i < iCount; i++){
        FileInfo* pFileInfo = new FileInfo;
        memcpy(pFileInfo, pdu->caMsg + i * sizeof(FileInfo), sizeof(FileInfo));
        pFileList.append(pFileInfo);
    }
    Index::getInstance().getFile()->updateFileList(pFileList);
}

void ResHandler::deleteDir(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));

    if(ret){
        QMessageBox::information(Index::getInstance().getFile(), "删除文件夹", "删除成功");
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(Index::getInstance().getFile(), "删除文件夹", "删除失败");
    }
}

void ResHandler::deleteFile(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));

    if(ret){
        QMessageBox::information(Index::getInstance().getFile(), "删除文件", "删除成功");
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(Index::getInstance().getFile(), "删除文件", "删除失败");
    }
}

void ResHandler::renameFile(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));

    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(Index::getInstance().getFile(), "重命名文件", "重命名失败");
    }
}

void ResHandler::moveFile(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));

    if(ret){
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(Index::getInstance().getFile(), "移动文件", "移动失败");
    }

}

void ResHandler::uploadFile(PDU *pdu)
{
    int ret;
    memcpy(&ret, pdu->caData, sizeof(int));
    qDebug() << "ResHandler::uploadFile ret:" << ret;
    if(ret == -1){
        QMessageBox::information(Index::getInstance().getFile(), "上传文件", "打开文件失败");
    }else if(ret == 1){
        QMessageBox::information(Index::getInstance().getFile(), "上传文件", "已有文件在上传");
    }else if(ret == 0){
        qDebug() << "ResHandler::uploadFile ret == 0 " << ret;
        Index::getInstance().getFile()->uploadFile();
    }else if(ret == 2){
        QMessageBox::information(Index::getInstance().getFile(), "上传文件", "上传成功");
        Index::getInstance().getFile()->flushFile();
        Index::getInstance().getFile()->m_bUpload = false;
    }else{
        return;
    }
}

void ResHandler::uploadFileData(PDU *pdu)
{
    qDebug() <<"--------";
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));

    if(ret){
        QMessageBox::information(Index::getInstance().getFile(), "上传文件", "上传成功");
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(Index::getInstance().getFile(), "上传文件", "上传失败");
    }
    Index::getInstance().getFile()->m_bUpload = false;
}

void ResHandler::downloadFile(PDU *pdu)
{
    qDebug() << "ResHandler::downloadFile start";
    int ret;
    memcpy(&ret, pdu->caData, sizeof(int));
    if(ret == 1){
        QMessageBox::information(Index::getInstance().getFile(), "下载文件", "已有文件正在下载");
        return;
    }else if(ret == -1){
        QMessageBox::information(Index::getInstance().getFile(), "下载文件", "下载文件失败");
        return;
    }
    qint64 iFileSize;
    memcpy(&iFileSize, pdu->caData + sizeof (int), sizeof(qint64));
    Index::getInstance().getFile()->downloadFile(iFileSize);
    qDebug() << "ResHandler::downloadFile end";
}

void ResHandler::downloadFileData(PDU *pdu)
{
    qDebug() << "ResHandler::downloadFileData start";
    Index::getInstance().getFile()->downloadFileData(pdu->caMsg, pdu->uiMsgLen);
    qDebug() << "ResHandler::downloadFileData end";
}

void ResHandler::shareFile(PDU *pdu)
{
    QMessageBox::information(Index::getInstance().getFile(), "分享文件", "文件已分享");
}

void ResHandler::shareFileRequest(PDU *pdu)
{
    QString strSharePath = pdu->caMsg;
    qDebug() << "shareFileRequest strSharePath:" << strSharePath;
    int index = strSharePath.lastIndexOf('/');
    QString strFileName = strSharePath.right(strSharePath.size() - index - 1);
    int ret = QMessageBox::question(
                Index::getInstance().getFile()->getShareFile(),
                "分享文件",
                QString("%1分享文件%2\n是否接收？").arg(pdu->caData).arg(strFileName));
    if(ret != QMessageBox::Yes){
        return;
    }
    PDU* respdu = mkPDU(pdu->uiMsgLen);
    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST;
    memcpy(respdu->caData, Client::getInstance().m_login_name.toStdString().c_str(), 32);
    qDebug() << "respdu->caData :" << respdu->caData;
    memcpy(respdu->caMsg, pdu->caMsg, pdu->uiMsgLen);
     qDebug() << "respdu->caMsg :" << respdu->caMsg;
    Client::getInstance().sendPDU(respdu);


}

void ResHandler::shareFileAgree(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));

    if(ret){
        QMessageBox::information(Index::getInstance().getFile(), "分享文件", "分享成功");
        Index::getInstance().getFile()->flushFile();
    }else{
        QMessageBox::information(Index::getInstance().getFile(), "分享文件", "分享失败");
    }
}
