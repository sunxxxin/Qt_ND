#include "operatedb.h"
#include "server.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OperateDB::getInstance().connect();
    Server w;
//    w.show();
    return a.exec();
}
