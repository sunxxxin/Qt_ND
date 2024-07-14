#ifndef SERVER_H
#define SERVER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QWidget
{
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);
    ~Server();
    //加载配置文件
    void loadConfig();
    static Server& getInstance();
    QString getRootPath();
private:
    Server(const Server& instance) = delete;
    Server& operator = (const Server&) = delete;
    Ui::Server *ui;
    QString m_strIp;
    quint16 m_usPort;
    QString m_strRootPath;
};
#endif // SERVER_H
