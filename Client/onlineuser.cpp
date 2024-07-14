#include "onlineuser.h"
#include "ui_onlineuser.h"
#include "client.h"
OnlineUser::OnlineUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

void OnlineUser::uodateOnlineUserWigetList(QStringList onlineUserList)
{
    ui->onlineUser_LW->clear();
    ui->onlineUser_LW->addItems(onlineUserList);
}

void OnlineUser::on_onlineUser_LW_itemDoubleClicked(QListWidgetItem *item)
{
    //获取当前登录用户名和选中的用户名
    QString strCurName = Client::getInstance().m_login_name;
    QString strTarName = item->text();
    qDebug() << "添加好友请求：当前用户：" << strCurName
             << "目标用户：" << strTarName;

    //构建PDU并发送给服务器
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADDFRIEND_REQUEST;
    memcpy(pdu->caData, strCurName.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, strTarName.toStdString().c_str(), 32);
    Client::getInstance().sendPDU(pdu);
}
