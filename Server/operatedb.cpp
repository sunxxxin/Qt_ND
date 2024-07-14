#include "operatedb.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
OperateDB &OperateDB::getInstance()
{
    static OperateDB instance;
    return instance;
}

void OperateDB::connect()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("mydb");
    m_db.setPort(3306);
    m_db.setUserName("root");
    m_db.setPassword("123456");
    if(m_db.open()){
        qDebug() << "数据库连接成功";
    }else{
        qDebug() << "数据库连接失败" << m_db.lastError().text();
    }

}

OperateDB::~OperateDB()
{
    m_db.close();
}

bool OperateDB::handleRegist(char *name, char *pwd)
{
    if(name == NULL || pwd == NULL){
        return false;
    }
    //查询用户是否存在
    QString sql = QString("select * from user_info where name = '%1'").arg(name);
    QSqlQuery q;
    if(!q.exec(sql) || q.next()){//执行失败或存在数据返回false
        return false;
    }
    //插入一个用户数据
    sql = QString("insert into user_info(name, pwd) values('%1', '%2')").arg(name).arg(pwd);
    return q.exec(sql);
}

bool OperateDB::handleLogin(char *name, char *pwd)
{
    if(name == NULL || pwd == NULL){
        return false;
    }
    //查询用户是否存在
    QString sql = QString("select * from user_info where name = '%1' and pwd = '%2'").arg(name).arg(pwd);
    QSqlQuery q;
    if(!q.exec(sql) || !q.next()){//执行失败或存在数据返回false
        return false;
    }
    sql = QString("update user_info set online = 1 where  name = '%1' and pwd = '%2'").arg(name).arg(pwd);
    return q.exec(sql);
}

void OperateDB::handOffline(const char *name)
{
    if(name == NULL){
        return;
    }
    QString sql = QString("update user_info set online = 0 where  name = '%1'").arg(name);
    QSqlQuery q;
    q.exec(sql);
}

int OperateDB::handleFindUser(const char *name)
{
    if(name == NULL){
        return -1;
    }
    QString sql = QString("select online from user_info where name = '%1'").arg(name);
    QSqlQuery q;
    q.exec(sql);
    if(!q.next()){
        return -1;
    }
    return q.value(0).toInt();//0离线 1在线
}

QStringList OperateDB::handleOnlineUser()
{
    QString sql = QString("select name from user_info where online = 1");
    QSqlQuery q;
    q.exec(sql);
    QStringList onlineUserList;
    while(q.next()){
        onlineUserList.append(q.value(0).toStringList());
    }
    return onlineUserList;
}

int OperateDB::handleAddFriend(char *curName, char *tarName)
{
    if(curName == NULL || tarName == NULL) return -1;
    QString sql = QString(R"(select * from friend
            where(
                  user_id =(select id from user_info where name = '%1')
                  and
                  friend_id =(select id from user_info where name = '%2')
                  )
            or(
                  friend_id =(select id from user_info where name = '%3')
                  and
                  user_id =(select id from user_info where name = '%4')
              );)").arg(curName).arg(tarName).arg(curName).arg(tarName);
    QSqlQuery q;
    q.exec(sql);
    if(q.next()){
        return -2;//代表已经是好友
    }
    QString sql1 = QString("select online from user_info where name = '%1'").arg(tarName);
    q.exec(sql1);
    if(q.next()){
        return q.value(0).toInt();//1在线 0 离线
    }
    return -1;
}

void OperateDB::handleAddFriendAgree(char *curName, char *tarName)
{
    if(curName == NULL || tarName == NULL) return;
    QString sql = QString(R"(insert into friend(user_id, friend_id)
                          select u1.id, u2.id from user_info u1, user_info u2
                          where u1.name = '%1' and u2.name = '%2';)").arg(curName).arg(tarName);
    QSqlQuery q;
    q.exec(sql);
}

QStringList OperateDB::handleFlushOnlineFriend(char *curName)
{
    QStringList res;
    res.clear();
    if(curName == NULL ) return res;
    QString sql = QString(R"(select name from user_info
                          where id in (
                                        select user_id from friend where friend_id = (select id from user_info where name = '%1')
                                        union
                                        select friend_id from friend where user_id = (select id from user_info where name = '%1')
                                      ) and online = 1)").arg(curName);
    QSqlQuery q;
    q.exec(sql);
    while(q.next()){
        res.append(q.value(0).toStringList());
    }
    return res;
}

bool OperateDB::handleDeleteFriend(char *curName, char *tarName)
{
    if(curName == NULL || tarName == NULL) return false;
    QString sql = QString(R"(select * from friend
            where(
                  user_id =(select id from user_info where name = '%1')
                  and
                  friend_id =(select id from user_info where name = '%2')
                  )
            or(
                  friend_id =(select id from user_info where name = '%3')
                  and
                  user_id =(select id from user_info where name = '%4')
              );)").arg(curName).arg(tarName).arg(curName).arg(tarName);
    QSqlQuery q;
    q.exec(sql);
    if(!q.next()){
        return false;//代表已经不是好友
    }
    sql = QString(R"(delete from friend
                where(
                      user_id =(select id from user_info where name = '%1')
                      and
                      friend_id =(select id from user_info where name = '%2')
                      )
                or(
                      friend_id =(select id from user_info where name = '%3')
                      and
                      user_id =(select id from user_info where name = '%4')
                  );)").arg(curName).arg(tarName).arg(curName).arg(tarName);
    q.exec(sql);
    return true;
}

OperateDB::OperateDB(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");

}
