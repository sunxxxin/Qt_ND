#include "client.h"
#include "ui_client.h"
#include "protocol.h"
#include "index.h"
#include <QFile>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>


Client::Client(QWidget *parent): QWidget(parent), ui(new Ui::Client)
{
    ui->setupUi(this);

    loadConfig();
    //关联连接成功的信号和处理信号的槽函数
    connect(&m_tcpSocket, &QTcpSocket::connected, this, &Client::showConnect);
    //客户端发送请求的槽函数
    connect(&m_tcpSocket, &QTcpSocket::readyRead, this,&Client::recvMsg);

    //连接到服务器
    m_tcpSocket.connectToHost(QHostAddress(strIp),usPort);

    rh = new ResHandler;
}

Client::~Client()
{
    delete ui;
    delete rh;
}

void Client::loadConfig()
{
    QFile file(":/client.config");//固定冒号开头
    if(file.open(QIODevice::ReadOnly)){//只读打开文件
        QByteArray baData = file.readAll();
        QString strData = QString(baData);
        QStringList strList = strData.split("\r\n");
        strIp = strList.at(0);
        usPort = strList.at(1).toUShort();//端口号转数字
        m_strRootPath = strList.at(2);
        qDebug() << "打开服务器配置文件ip:" << strIp << "port:" << usPort << "RootPath:" << m_strRootPath;
        file.close();
    }else{
        qDebug() << "打开配置失败";
    }
}

Client &Client::getInstance()
{
    static Client instance;
    return instance;
}

QTcpSocket& Client::getSocket()
{
    return m_tcpSocket;
}


QTcpSocket &Client::getTcpSocket()
{
    return m_tcpSocket;
}

//PDU *Client::readPDU()
//{
//    qDebug() << "recvMsg 接收的消息长度" << m_tcpSocket.bytesAvailable();
//    uint uiPDULen = 0;
//    //读取协议总长度uiPDULen
//    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));
//    uint uiMsgLen = uiPDULen - sizeof(PDU);
//    PDU* pdu = mkPDU(uiMsgLen);
//    //读取除了协议总长度以外的内容
//    m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));
//    qDebug() << "recvMsg消息类型：" << pdu->uiMsgType
//             << "参数内容：" << pdu->caData
//             << "消息内容：" << pdu->caMsg;
//    return pdu;
//}

void Client::sendPDU(PDU *pdu)
{
    m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Client::handlePDU(PDU *pdu)
{
    qDebug() << "recvMsg消息类型：" << pdu->uiMsgType
             << "参数内容1：" << pdu->caData
             << "参数内容2：" << pdu->caData + 32
             << "消息内容：" << pdu->caMsg;
    switch (pdu->uiMsgType) {
    case ENUM_MSG_TYPE_REGIST_RESPEND:{
        rh->regist(pdu);
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPEND:{
        rh->login(pdu, m_login_name);
        break;
    }
    case ENUM_MSG_TYPE_FINDUSER_RESPEND:{
        rh->findUser(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ONLINEUSER_RESPEND:{
        rh->onlineUser(pdu, m_login_name);
        break;
    }
    case ENUM_MSG_TYPE_ADDFRIEND_RESPEND:{
        rh->addFriend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADDFRIEND_REQUEST:{
        rh->addFriendRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADDFRIEND_AGREE_RESPEND:{
        rh->addFriendAgree(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_ONLINE_FRIEND_RESPEND:{
        rh->flushOnlineFriend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPEND:{
        rh->deleteFriend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CHAT_REQUEST:{
        rh->chat(pdu);
        break;
    }
    case ENUM_MSG_TYPE_MKDIR_RESPEND:{
        rh->mkdir(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPEND:{
        rh->flushFile(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_RESPEND:{
        rh->deleteDir(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DEL_FILE_RESPEND:{
        rh->deleteFile(pdu);
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_RESPEND:{
        rh->renameFile(pdu);
        break;
    }
    case ENUM_MSG_TYPE_MOVE_FILE_RESPEND:{
        rh->moveFile(pdu);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPEND:{
        rh->uploadFile(pdu);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPEND:{
        rh->uploadFileData(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPEND:{
        rh->downloadFile(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA_RESPEND:{
        rh->downloadFileData(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_RESPEND:{
        rh->shareFile(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:{
        rh->shareFileRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPEND:{
        rh->shareFileAgree(pdu);
        break;
    }
    default:
        break;
    }
}

QString Client::getRootPath()
{
    return m_strRootPath;
}

void Client::showConnect()
{
    QMessageBox::information(this,"连接服务器", "连接服务器成功");
}

void Client::recvMsg()
{
    qDebug() << "\n\n\nrecvMsg 接收的消息长度" << m_tcpSocket.bytesAvailable();
    QByteArray data = m_tcpSocket.readAll();
    buffer.append(data);
    qDebug() << "buffer.size() : " << buffer.size() << "sizeof(PDU)" << sizeof(PDU);
    while(buffer.size() >= int(sizeof(PDU))){//判断是否够一个完整的PDU，为了能取PDU中的协议长度
        PDU* pdu = (PDU*)buffer.data();
        if(buffer.size() < int(pdu->uiPDULen)){//判断是否够一个完整的协议请求
            break;
        }
        handlePDU(pdu);
        buffer.remove(0, pdu->uiPDULen);
    }

}


//void Client::on_pushButton_clicked()
//{
//    QString strMsg = ui->lineEdit->text();
//    if(strMsg.isEmpty()){
//        QMessageBox::information(this,"发送消息", "发送消息不能为空");
//        return;
//    }
//    PDU* pdu = mkPDU(strMsg.toStdString().size());
//    pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
//    memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.toStdString().size());
//    m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
//    free(pdu);
//    pdu = NULL;
//}


void Client::on_regist_PB_clicked()
{
    QString strName = ui->name_LE->text();
    QString strPwd = ui->pwd_LE->text();
    if(strName.isEmpty()||
            strPwd.isEmpty()||
            strName.toStdString().size() > 32||
            strPwd.toStdString().size() > 32){
        QMessageBox::information(this, "注册", "用户或密码错误");
        return;
    }
    //构造并初始化pdu
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
    memcpy(pdu->caData, strName.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
    //发送pdu
    sendPDU(pdu);
}




void Client::on_login_PB__clicked()
{
    QString strName = ui->name_LE->text();
    QString strPwd = ui->pwd_LE->text();
    if(strName.isEmpty()||
            strPwd.isEmpty()||
            strName.toStdString().size() > 32||
            strPwd.toStdString().size() > 32){
        QMessageBox::information(this, "登录", "用户或密码错误");
        return;
    }
    //构造并初始化pdu
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
    memcpy(pdu->caData, strName.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
    //发送pdu
    sendPDU(pdu);

}
