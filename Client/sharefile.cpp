#include "sharefile.h"
#include "ui_sharefile.h"
#include "index.h"
#include "client.h"

ShareFile::ShareFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

ShareFile::~ShareFile()
{
    delete ui;
}

void ShareFile::updateFriend_LW()
{
    ui->onlineFriend_LW->clear();
    QListWidget* friend_LW = Index::getInstance().getFriend()->getFriend_LW();
    for(int i = 0; i < friend_LW->count(); i++){
        QListWidgetItem* friendItem = friend_LW->item(i);
        QListWidgetItem* newItem = new QListWidgetItem(*friendItem);
        ui->onlineFriend_LW->addItem(newItem);
    }
}

void ShareFile::on_allSelect_PB_clicked()
{
    for(int i = 0; i < ui->onlineFriend_LW->count(); i++){
        ui->onlineFriend_LW->item(i)->setSelected(true);
    }
}

void ShareFile::on_cancelSelect_PB_clicked()
{
    for(int i = 0; i < ui->onlineFriend_LW->count(); i++){
        ui->onlineFriend_LW->item(i)->setSelected(false);
    }
}

void ShareFile::on_ok_PB_clicked()
{
    //获取当前路径和当前用户名
    QString strCurPath = Index::getInstance().getFile()->m_strCurPath;
    QString strPath = strCurPath + "/" + Index::getInstance().getFile()->m_strShareFileName;
    QString strCurName = Client::getInstance().m_login_name;
    //获取选择的好友
    QList<QListWidgetItem*> pItems = ui->onlineFriend_LW->selectedItems();
    int friendNum = pItems.size();
    //构建pdu，当前用户名和好友数量放入caData，好友名和文件路径放入caMsg
    PDU* pdu = mkPDU(friendNum * 32 + strPath.toStdString().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    memcpy(pdu->caData, strCurName.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, &friendNum, sizeof(int));

    for(int i = 0; i < friendNum; i++){
        memcpy(pdu->caMsg + i * 32, pItems.at(i)->text().toStdString().c_str(), 32);
    }
    memcpy(pdu->caMsg + friendNum * 32, strPath.toStdString().c_str(), strPath.toStdString().size());
    Client::getInstance().sendPDU(pdu);
}
