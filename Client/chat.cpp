#include "chat.h"
#include "ui_chat.h"
#include "protocol.h"
#include "client.h"
Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);

}

Chat::~Chat()
{
    delete ui;
}

void Chat::updateShow_LE(QString strMsg)
{
    ui->show_TE->append(strMsg);

}

void Chat::on_send_PB_clicked()
{
    //获取发送的消息内容
    QString strMsg = ui->input_LE->text();
    QString strMyMsg = QString("%1: %2").arg(Client::getInstance().m_login_name).arg(strMsg);
    updateShow_LE(strMyMsg);
    if(strMsg.isEmpty()){
        return;
    }
    PDU* pdu = mkPDU(strMsg.toStdString().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_CHAT_REQUEST;
    //当前用户名和聊天用户名放入caData
    memcpy(pdu->caData, Client::getInstance().m_login_name.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, m_strChatName.toStdString().c_str(), 32);
    //聊天内容放入caMsg
    memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.toStdString().size());
    Client::getInstance().sendPDU(pdu);
    ui->input_LE->clear();
}
