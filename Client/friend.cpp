#include "friend.h"
#include "ui_friend.h"
#include "protocol.h"
#include "client.h"
#include <QInputDialog>
#include <QDebug>
#include <QMessageBox>
Friend::Friend(QWidget *parent) :QWidget(parent),ui(new Ui::Friend)
{
    ui->setupUi(this);
    m_pOnlineUser = new OnlineUser;
    m_pChat = new Chat;
    flushFriend();
}

Friend::~Friend()
{
    delete ui;
    delete m_pOnlineUser;
    delete m_pChat;
}

OnlineUser *Friend::get_m_pOnlineUser()
{
    return m_pOnlineUser;
}

Chat *Friend::getChat()
{
    return m_pChat;
}

void Friend::flushFriend()
{
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_ONLINE_FRIEND_REQUEST;
    memcpy(pdu->caData, Client::getInstance().m_login_name.toStdString().c_str(), 32);
    Client::getInstance().sendPDU(pdu);
}

void Friend::updateFriendLW(QStringList friendList)
{
    ui->onlineFriend_LW->clear();
    ui->onlineFriend_LW->addItems(friendList);
}

QListWidget *Friend::getFriend_LW()
{
    return ui->onlineFriend_LW;
}

void Friend::on_findUser_PB_clicked()
{
    QString name = QInputDialog::getText(this, "查找", "用户名:");
    qDebug() << "name: " << name;
    //构造并初始化pdu
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FINDUSER_REQUEST;
    memcpy(pdu->caData, name.toStdString().c_str(), 32);
    Client::getInstance().getSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}



void Friend::on_flushFriend_PB_clicked()
{
    flushFriend();
}

void Friend::on_onlineUser_PB_clicked()
{
    if(m_pOnlineUser->isHidden()){
        m_pOnlineUser->show();
    }
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ONLINEUSER_REQUEST;
    Client::getInstance().getSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Friend::on_delFriend_PB_clicked()
{
    QListWidgetItem* pItem = ui->onlineFriend_LW->currentItem();
    if(!pItem){
        QMessageBox::information(this, "删除好友", "请选择要删除的好友");
        return;
    }
    QString strTarName = pItem->text();
    int ret = QMessageBox::question(this, "删除好友", QString("是否确定删除好友'%1'").arg(strTarName));
    if(ret != QMessageBox::Yes){
        return;
    }
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    QString strCurName = Client::getInstance().m_login_name;
    memcpy(pdu->caData, strCurName.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, strTarName.toStdString().c_str(), 32);
    Client::getInstance().sendPDU(pdu);



}

void Friend::on_chat_PB_clicked()
{
    QListWidgetItem* pItem = ui->onlineFriend_LW->currentItem();
    if(!pItem){
        QMessageBox::information(this, "聊天", "请选择要聊天的好友");
        return;
    }
    if(m_pChat->isHidden()){
        m_pChat->show();
    }
    m_pChat->m_strChatName = pItem->text();
    m_pChat->setWindowTitle(m_pChat->m_strChatName);
}
