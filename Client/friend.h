#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTcpSocket>
#include "onlineuser.h"
#include "chat.h"
namespace Ui {
class Friend;
}

class Friend : public QWidget
{
    Q_OBJECT

public:
    explicit Friend(QWidget *parent = nullptr);
    ~Friend();
    OnlineUser* get_m_pOnlineUser();
    Chat* getChat();
    void flushFriend();
    void updateFriendLW(QStringList friendList);
    QListWidget* getFriend_LW();
private slots:
    void on_findUser_PB_clicked();

    void on_onlineUser_PB_clicked();

    void on_flushFriend_PB_clicked();



    void on_delFriend_PB_clicked();

    void on_chat_PB_clicked();

private:
    Ui::Friend *ui;
    OnlineUser* m_pOnlineUser;
    Chat* m_pChat;
};

#endif // FRIEND_H
