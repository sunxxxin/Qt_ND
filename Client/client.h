#ifndef CLIENT_H
#define CLIENT_H

#include "reshandler.h"
#include "protocol.h"

#include <QWidget>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:
    ~Client();

    //加载配置文件的函数声明
    void loadConfig();
    QTcpSocket& getTcpSocket();
    QTcpSocket& getSocket();
    //单例模式
    static Client& getInstance();



    ResHandler* rh;
//    PDU* readPDU();
    void sendPDU(PDU* pdu);
    void handlePDU(PDU* pdu);
    QString m_login_name;
    QString getRootPath();
    QByteArray buffer;
public slots:
    //声明链接信号的处理槽函数
    void showConnect();
    void recvMsg();

private slots:
//    void on_pushButton_clicked();

    void on_regist_PB_clicked();
    void on_login_PB__clicked();

private:
    //实现单例模式，私有化构造函数，删除拷贝构造函数与拷贝赋值运算符
    Client(QWidget *parent = nullptr);
    Client(const Client& instance) = delete;
    Client& operator = (const Client&) = delete;

    Ui::Client *ui;
    //IP和端口号成员变量
    QString strIp;
    quint16 usPort;
    QString m_strRootPath;
    //连接服务器，和服务器数据交互
    QTcpSocket m_tcpSocket;


};


#endif // CLIENT_H
