#ifndef ONLINEUSER_H
#define ONLINEUSER_H

#include <QListWidget>
#include <QWidget>

namespace Ui {
class OnlineUser;
}

class OnlineUser : public QWidget
{
    Q_OBJECT

public:
    explicit OnlineUser(QWidget *parent = nullptr);
    ~OnlineUser();
    void uodateOnlineUserWigetList(QStringList onlineUserList);


private slots:
    void on_onlineUser_LW_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::OnlineUser *ui;
};

#endif // ONLINEUSER_H
