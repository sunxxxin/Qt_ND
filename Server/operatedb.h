#ifndef OPERATEDB_H
#define OPERATEDB_H

#include <QObject>
#include <QSqlDatabase>

class OperateDB : public QObject
{
    Q_OBJECT
public:

    QSqlDatabase m_db;//数据库对象
    static OperateDB& getInstance();
    void connect();
    ~OperateDB();

    bool handleRegist(char* name, char* pwd);
    bool handleLogin(char* name, char* pwd);
    void handOffline(const char* name);
    int handleFindUser(const char* name);
    QStringList handleOnlineUser();
    int handleAddFriend(char* curName, char* tarName);
    void handleAddFriendAgree(char* curName, char* tarName);
    QStringList handleFlushOnlineFriend(char* curName);
    bool handleDeleteFriend(char* curName, char* tarName);

private:
    explicit OperateDB(QObject *parent = nullptr);
    OperateDB(const OperateDB& instance) = delete;
    OperateDB& operator = (const OperateDB& instance) = delete;


signals:

};

#endif // OPERATEDB_H
